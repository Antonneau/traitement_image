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
  float (*tmp_transfer)[transfer_cols][3] = (float (*)[transfer_cols][3])data_transfer;
  // Getting the components
  for(int i = 0; i < transfer_rows; i++){
    for (int j = 0; j < transfer_cols; j++){
      for(int c = 0; c < 3; c++){
        tmp_transfer[i][j][c] = pnm_get_component(transfer, i, j, c);
      }
    }
  }
  
  // Processing
  for(int i = 0; i < transfer_rows; i++){
    for (int j = 0; j < transfer_cols; j++){
      float tmp_transfer_lms[3] = {0.0, 0.0, 0.0};
      float tmp_transfer_lab[3] = {0.0, 0.0, 0.0};
      // RGB -> LMS
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_lms = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lms += RGB2LMS[mat_line][mat_col] * tmp_transfer[i][j][mat_col];
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
        tmp_transfer_lab[mat_line] = tmp_lab;
      }
      // L-alpha-beta -> LMS
      for(int mat_line = 0; mat_line < D; mat_line++){
        float tmp_loglms = 0.0;
        // L-alpha-beta -> log(LMS)
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_loglms += LAB2LOGLMS[mat_line][mat_col] * tmp_transfer_lab[mat_col];
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
        /*if (tmp_rgb < 0.0)
          tmp_rgb = 0.0;*/
        tmp_transfer[i][j][mat_line] = tmp_rgb;
      }
    }
  }

  // Printing pixels in the picture
  for(int i = 0; i < transfer_rows; i++){
    for (int j = 0; j < transfer_cols; j++){
      for(int c = 0; c < D; c++){
        pnm_set_component(res, i, j, c, (unsigned short) tmp_transfer[i][j][c] );
        //printf("%f ", tmp[i][j][c]);
      }
      //printf("\n");
    }
  }

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
