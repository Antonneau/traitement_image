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
rgb_to_lab(pnm ims, float *res)
{
  float (*tmp)[pnm_get_height(ims)][3] = (float (*)[pnm_get_height(ims)][3])res;
  for(int i = 0; i < pnm_get_height(ims); i++){
    for (int j = 0; j < pnm_get_width(ims); j++){
      // rgb
      for(int c = 0; c < 3; c++){
        tmp[i][j][c] = (float) pnm_get_component(ims, i, j, c);
      }
      // rgb to lms
      float tmp_lms;
      float tmp_lab;
      for(int mat_line = 0; mat_line < D; mat_line++){
        tmp_lms = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lms += RGB2LMS[mat_line][mat_col] * tmp[i][j][mat_col];
        }
        // lms to log lms
        tmp[i][j][mat_line] = log(tmp_lms);
        // lab
        tmp_lab = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lab += LOGLMS2LAB[mat_line][mat_col] * tmp[i][j][mat_col];
        }
        tmp[i][j][mat_line] = tmp_lab;

      }
    }
  }
}

void
lab_to_rgb(pnm imd, float *res)
{
  float (*tmp)[pnm_get_height(imd)][3] = (float (*)[pnm_get_height(imd)][3])res;
  for(int i = 0; i < pnm_get_height(imd); i++){
    for (int j = 0; j < pnm_get_width(imd); j++){
      // lab to log lms
      float tmp_lab;
      float tmp_loglms;
      float tmp_lms;
      for(int mat_line = 0; mat_line < D; mat_line++){
        // lab
        tmp_lab = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lab += LAB2LOGLMS[mat_line][mat_col] * tmp[i][j][mat_col];
        }
        tmp_loglms = 1.0;
        for(int pow = 0; pow < 10-1; pow++){
          tmp_loglms *= tmp_lab;
        }

        tmp[i][j][mat_line] = tmp_lms;

        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lms += LMS2RGB[mat_line][mat_col] * tmp[i][j][mat_col];
        }
        // lms to log lms
        tmp[i][j][mat_line] = tmp_lms;
      }
      for(int c = 0; c < 3; c++){
        pnm_set_component(imd, i, j, c, (short) tmp[i][j][c]);
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
  float *data_source = malloc(source_rows * source_cols * 3 * sizeof(float));
  if(data_source == NULL)
    exit(EXIT_FAILURE);
  float *data_transfer = malloc(transfer_rows * transfer_cols * 3 * sizeof(float));
  if(data_transfer == NULL)
    exit(EXIT_FAILURE);
  // RGB to L-alpha-beta
  printf("Computing l-alpha-beta for source image...\n");
  rgb_to_lab(source, data_source);
  printf("Source done. Now computing for transfer image...\n");
  rgb_to_lab(transfer, data_transfer);

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

  // Result image
  pnm res = pnm_new(transfer_cols, transfer_rows, PnmRawPpm);
  lab_to_rgb(res, data_transfer);
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
