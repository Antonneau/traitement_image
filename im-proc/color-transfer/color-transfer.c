/**
 * @file color-transfert
 * @brief transfert color from source image to target image.
 *        Method from Reinhard et al. : 
 *        Erik Reinhard, Michael Ashikhmin, Bruce Gooch and Peter Shirley, 
 *        'Color Transfer between Images', IEEE CGA special issue on 
 *        Applied Perception, Vol 21, No 5, pp 34-41, September - October 2001
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <bcl.h>

#define D 3

float RGB2LMS[D][D] = {
  {0.3811, 0.5783, 0.0402}, 
  {0.1967, 0.7244, 0.0782},  
  {0.0241, 0.1288, 0.8444}
};

float LOGLMS2LAB[D][D] = {
  {0.5573,  0.5773,  0.5773},
  {0.4082,  0.4082, -0.8164},
  {0.7071, -0.7071,     0.0}
};

float LAB2LOGLMS[D][D] = {
  {0.5573,  0.4082,  0.7071},
  {0.5573,  0.4082, -0.7071},
  {0.5573, -0.8164,     0.0}
};

float LMS2RGB[D][D] = {
  { 4.4679, -3.5873,  0.1193},
  {-1.2186,  2.3809, -0.1624},
  { 0.0497, -0.2439,  1.2045}
};

// Normalization values
float minValue[D] = { INFINITY,  INFINITY,  INFINITY};
float maxValue[D] = {-INFINITY, -INFINITY, -INFINITY};
float max = 255.0;
float min = 0.0;

/**
 * @brief Initializes the data list from the pixels in "ims"
 * @param ims the input image
 * @param data the data list to initialize
 */
void
init_data(pnm ims, float* data)
{
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  float (*tmp)[cols][3] = (float (*)[cols][3])data;
  for(int i = 0; i < rows; i++){
    for (int j = 0; j < cols; j++){
      for(int c = 0; c < 3; c++){
        tmp[i][j][c] = pnm_get_component(ims, i, j, c);
      }
    }
  }
}

/**
 * @brief Converts the data from RGB color space to L-alpha-beta color space
 * @param ims the input image (where we get the dimensions)
 * @param data the RGB color space data to convert
 */
void
rgb_to_lalphabeta(pnm ims, float* data)
{
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  float (*tmp)[cols][3] = (float (*)[cols][3])data;
  for(int i = 0; i < rows; i++){
    for (int j = 0; j < cols; j++){
      float tmp_transfer_lms[3] = {0.0, 0.0, 0.0};
      // RGB -> LMS
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_lms = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lms += RGB2LMS[mat_line][mat_col] * tmp[i][j][mat_col];
        }
        tmp_transfer_lms[mat_line] = tmp_lms;
      }
      // LMS -> L-alpha-beta
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_lab = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          float tmp_loglms = log10f(tmp_transfer_lms[mat_col]);
          if(tmp_transfer_lms[mat_col] == 0.0)
            tmp_loglms = 0.0;
          tmp_lab += LOGLMS2LAB[mat_line][mat_col] * tmp_loglms;
        }
        tmp[i][j][mat_line] = tmp_lab;
      }
    }
  }
}

/**
 * @brief Converts the data from L-alpha-beta color space to RGB color space
 * @param ims the input image (where we get the dimensions)
 * @param data the L-alpha-beta color space data to convert
 */
void
lalphabeta_to_rgb(pnm ims, float* data)
{
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  float (*tmp)[cols][3] = (float (*)[cols][3])data;
  for(int i = 0; i < rows; i++){
    for (int j = 0; j < cols; j++){
      float tmp_transfer_lms[3] = {0.0, 0.0, 0.0};
      // L-alpha-beta -> LMS
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_loglms = 0.0;
        // L-alpha-beta -> log(LMS)
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_loglms += LAB2LOGLMS[mat_line][mat_col] * tmp[i][j][mat_col];
        }
        // log(LMS) -> LMS
        tmp_transfer_lms[mat_line] = pow(10.0, tmp_loglms);
      }
      // LMS -> RGB
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_rgb = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_rgb += LMS2RGB[mat_line][mat_col] * tmp_transfer_lms[mat_col];
        }
        
        if (tmp_rgb < minValue[mat_line]){
          minValue[mat_line] = tmp_rgb;
        }
        if (tmp_rgb > maxValue[mat_line]){
          maxValue[mat_line] = tmp_rgb;
        }
        
        tmp[i][j][mat_line] = tmp_rgb;
      }
    }
  }
}

/**
 * @brief Computes the average of each channel in "data"
 * @param ims the input image (where we get the dimensions)
 * @param data the data used to get the average
 * @param avg the computed average (return value)
 */
void
compute_average(pnm ims, float* data, float* avg)
{
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  float (*tmp)[cols][3] = (float (*)[cols][3])data;
  for(int i = 0; i < rows; i++){
    for (int j = 0; j < cols; j++){
      for(int c = 0; c < D; c++){
        avg[c] += tmp[i][j][c];
      }
    }
  }
  for(int c = 0; c < D; c++){
    avg[c] = avg[c] / (rows*cols);
  }
}

/**
 * @brief Computes the standard deviation of each channel in "data"
 * @param ims the input image (where we get the dimensions)
 * @param data the data used to get the standard deviation
 * @param avg the average used in the standard deviation formula
 * @param res the computed standard deviation (return value)
 */
void
compute_standard_deviation(pnm ims, float* data, float* avg, float* res)
{
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  float (*tmp)[cols][3] = (float (*)[cols][3])data;
  for(int i = 0; i < rows; i++){
    for (int j = 0; j < cols; j++){
      for(int c = 0; c < D; c++){
        res[c] += (tmp[i][j][c] - avg[c]) * (tmp[i][j][c] - avg[c]);
      }
    }
  }
  for(int c = 0; c < D; c++){
    res[c] = sqrt((1.0/rows*cols) * res[c]);
  }
}

/**
 * @brief Applies the color correction
 * @param ims the input image (where we get the dimensions)
 * @param data the data used to get the standard deviation
 * @param avg the average used in the standard deviation formula
 * @param res the computed standard deviation (return value)
 */
void
color_correction(pnm ims, float* data, float* avg_src, float* avg_trf, float* std_dev_src, float* std_dev_trf)
{
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  float (*tmp)[cols][3] = (float (*)[cols][3])data;
  for(int i = 0; i < rows; i++){
    for (int j = 0; j < cols; j++){
      for(int c = 0; c < D; c++){
        tmp[i][j][c] = (std_dev_src[c] / std_dev_trf[c]) * (tmp[i][j][c] - avg_trf[c]) + avg_src[c];
      }
    }
  }
}

void
construct_image(pnm ims, float* data)
{
  int rows = pnm_get_height(ims);
  int cols = pnm_get_width(ims);
  float (*tmp)[cols][3] = (float (*)[cols][3])data;
  for(int i = 0; i < rows; i++){
    for (int j = 0; j < cols; j++){
      for(int c = 0; c < D; c++){
        float norm = ((max - min) / (maxValue[c] - minValue[c])) * tmp[i][j][c]
                   + (min * maxValue[c] - max * minValue[c]) / (maxValue[c] - minValue[c]);
        pnm_set_component(ims, i, j, c, (unsigned short) norm);
      }
    }
  }
}

void
process(char *ims, char *imt, char* imd){
  // Source image (where we take the colors)
  pnm source = pnm_load(ims);
  int source_rows = pnm_get_height(source);
  int source_cols = pnm_get_width(source);
  // Transfer image (where we transfer the colors)
  pnm transfer = pnm_load(imt);
  int transfer_rows = pnm_get_height(transfer);
  int transfer_cols = pnm_get_width(transfer);

  // RGB List (transfer image)
  float *data_source = (float*)calloc(source_rows * source_cols * 3, sizeof(float));
  if(data_source == NULL)
    exit(EXIT_FAILURE);
  float *data_transfer = (float*)calloc(transfer_rows * transfer_cols * 3, sizeof(float));
  if(data_transfer == NULL)
    exit(EXIT_FAILURE);
    
  // Getting the components
  init_data(source, data_source);
  init_data(transfer, data_transfer);
  
  // RGB to L-alpha-beta conversion
  rgb_to_lalphabeta(source, data_source);
  rgb_to_lalphabeta(transfer, data_transfer);

  // Compute average
  float avg_src[3] = {0.0, 0.0, 0.0};
  float avg_trf[3] = {0.0, 0.0, 0.0};
  compute_average(source, data_source, avg_src);
  compute_average(transfer, data_transfer, avg_trf);

  float std_dev_src[3] = {0.0, 0.0, 0.0};
  float std_dev_trf[3] = {0.0, 0.0, 0.0};
  compute_standard_deviation(source, data_source, avg_src, std_dev_src);
  compute_standard_deviation(transfer, data_transfer, avg_trf, std_dev_trf);
  
  color_correction(transfer, data_transfer, avg_src, avg_trf, std_dev_src, std_dev_trf);

  // L-alpha-beta to RGB conversion
  lalphabeta_to_rgb(transfer, data_transfer);
  
  // Printing pixels in the picture
  pnm res = pnm_new(transfer_cols, transfer_rows, PnmRawPpm);
  construct_image(res, data_transfer);

  // Saving image
  pnm_save(res, PnmRawPpm, imd);

  pnm_free(source);
  pnm_free(transfer);
  pnm_free(res);
  free(data_source);
  free(data_transfer);
}

void
usage (char *s){
  fprintf(stderr, "Usage: %s <ims> <imt> <imd> \n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int
main(int argc, char *argv[]){
  if (argc != PARAM+1) usage(argv[0]);
  process(argv[1], argv[2], argv[3]);
  return EXIT_SUCCESS;
}
