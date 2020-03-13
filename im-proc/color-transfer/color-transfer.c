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
  {0.0141, -0.0141,     0.0}
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

void
lms_to_lab(pnm ims, float *res)
{
  for(int i = 0; i < pnm_get_height(ims); i++){
    for (int j = 0; j < pnm_get_width(ims); j++){
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_lab = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lab += LOGLMS2LAB[mat_line][mat_col] * res[i*pnm_get_height(ims) + j*3 + mat_col];
        }
        res[i*pnm_get_height(ims) + j*3 + mat_line] = tmp_lab;
      }
    }
  }
}

void
lab_to_lms(pnm ims, float *res)
{
  for(int i = 0; i < pnm_get_height(ims); i++){
    for (int j = 0; j < pnm_get_width(ims); j++){
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_loglms = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_loglms += LAB2LOGLMS[mat_line][mat_col] * res[i*pnm_get_height(ims) + j*3 + mat_col];
        }
        res[i*pnm_get_height(ims) + j*3 + mat_line] = pow(10.0, tmp_loglms);
      }
    }
  }
}

void
compute_average(pnm ims, float *src, float *res)
{
  //float (*tmp)[pnm_get_height(ims)][3] = (float (*)[pnm_get_height(ims)][3])src;
  for(int i = 0; i < pnm_get_height(ims); i++){
    float line_tot[3]= {0.0,0.0,0.0};
    for (int j = 0; j < pnm_get_width(ims); j++){
      for(int c = 0; c < 3; c++){
        line_tot[c] += src[i*j + j + c];
        printf("New value for transfer sum : %f\n", line_tot[c]);
      }
    }
    for(int c=0;c<3;c++)
      res[c]+= line_tot[c]/ (pnm_get_width(ims)* pnm_get_height(ims));
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
    
  // Result image
  pnm res = pnm_new(transfer_cols, transfer_rows, PnmRawPpm);
  
  float minValue[3] = {INFINITY, INFINITY, INFINITY};
  float maxValue[3] = {-INFINITY, -INFINITY, -INFINITY};
  for(int i = 0; i < transfer_rows; i++){
    for (int j = 0; j < transfer_cols; j++){
      for(int c = 0; c < 3; c++){
        data_transfer[i*transfer_rows + j*3 + c] = (float) pnm_get_component(transfer, i, j, c);
      }
      // RGB -> LMS
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_lms = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lms += RGB2LMS[mat_line][mat_col] * data_transfer[i*transfer_rows + j*3 + mat_col];
        }
        data_transfer[i*transfer_rows + j*3 + mat_line] = tmp_lms;
        /*if(data_transfer[i*transfer_rows + j*3 + mat_line] < 0.0)
          data_transfer[i*transfer_rows + j*3 + mat_line] = 0.0;*/
        //pnm_set_component(res, i, j, mat_line, data_transfer[i*transfer_rows + j*3 + mat_line]);
      }
      // LMS -> RGB
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_rgb = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_rgb += LMS2RGB[mat_line][mat_col] * data_transfer[i*transfer_rows + j*3 + mat_col];
        }
        data_transfer[i*transfer_rows + j*3 + mat_line] = tmp_rgb;
        
        if (minValue[mat_line] > tmp_rgb)
          minValue[mat_line] = tmp_rgb;
        if (maxValue[mat_line] < tmp_rgb)
          maxValue[mat_line] = tmp_rgb;
        //pnm_set_component(res, i, j, mat_line, data_transfer[i*transfer_rows + j*3 + mat_line]);
      }
      //printf("\n");
    }
  }
  
  for(int i = 0; i < transfer_rows; i++){
    for (int j = 0; j < transfer_cols; j++){
      for(int c = 0; c < D; c++){
        //printf("min : %f, %f, %f\nmax : %f, %f, %f\n", minValue[0], minValue[1], minValue[2], maxValue[0], maxValue[1], maxValue[2]);
        float norm = ((255 - 0) / (maxValue[c] - minValue[c])) * data_transfer[i*transfer_rows + j*3 + c] 
                   + (0 * maxValue[c] - 255 * minValue[c]) / (maxValue[c] - minValue[c]);
        pnm_set_component(res, i, j, c, norm);
      }
    }
  }
  /*
  for(int i = 0; i < pnm_get_height(transfer); i++){
    for (int j = 0; j < pnm_get_width(transfer); j++){
      for (int c = 0; c < D; c++){
        printf("%f, ", data_transfer[i*j + j*3 + c]);
      }
      printf("\n");
    }
  }
  */
  pnm_save(res, PnmRawPpm, imd);
  // <l-alpha-beta>
  /*float source_lab_avg[3] = {0.0, 0.0, 0.0};
  float transfer_lab_avg[3] = {0.0, 0.0, 0.0};
  compute_average(source, data_source, source_lab_avg);
  compute_average(transfer, data_transfer, transfer_lab_avg);
  
  for(int c = 0; c < 3; c++){
    printf("Computed average for :\nSource : %f\nTransfer : %f\n", source_lab_avg[c], transfer_lab_avg[c]);
  }*/

  /* TODO */
  // Calculer l*, a* et b*
  // Calculer l', a' et b'
  // Rajouter aux resultats précedents la moyenne de l', a' et b'

  /*
  for(int i = 0; i < pnm_get_height(res); i++){
    for (int j = 0; j < pnm_get_width(res); j++){
      for (int c = 0; c < D; c++){
        printf("%f, ", data_transfer[i*j + j*c + c]);
      }
      printf("\n");
    }
  }
  */

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
