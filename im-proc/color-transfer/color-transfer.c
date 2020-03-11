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

/*float LOGLMS2LAB_LEFT[D][D] = {
  {1.0/sqrt(3.0),             0,             0},
  {            0, 1.0/sqrt(6.0),             0},
  {            0,             0, 1.0/sqrt(2.0)}
};

float LOGLMS2LAB_RIGHT[D][D] = {
  {1.0,  1.0,  1.0},
  {1.0,  1.0, -2.0},
  {1.0, -1.0,  0.0}
};*/
/*
float LAB2LOGLMS[D][D] = {
  {sqrt(3.0)/3.0,              0, 0},
  {             0, sqrt(6.0)/6.0, 0},
  {             0,             0, 0}
};
*/
/*float LAB2LOGLMS_LEFT[D][D] = {
  {1.0,  1.0,  1.0},
  {1.0,  1.0, -1.0},
  {1.0, -2.0,  0.0}
};

float LAB2LOGLMS_RIGHT[D][D] = {
  {sqrt(3.0)/3.0,              0,             0},
  {             0, sqrt(6.0)/6.0,             0},
  {             0,             0, sqrt(2.0)/2.0}
};*/

float LMS2RGB[D][D] = {
  { 4.4679, -3.5873,  0.1193},
  {-1.2186,  2.3809, -0.1624},
  { 0.0497, -0.2439,  1.2045}
};

void
rgb_to_lab(pnm ims, float *res)
{
  float (*tmp)[pnm_get_height(ims)][3] = (float (*)[pnm_get_height(ims)][3])res;
  float LOGLMS2LAB[D][D] = {
    {1.0/sqrt(3.0),             0, 0},
    {            0, 1.0/sqrt(6.0), 0},
    {            0,             0, 0}
  };

  for(int i = 0; i < pnm_get_height(ims); i++){
    for (int j = 0; j < pnm_get_width(ims); j++){
      // rgb
      for(int c = 0; c < 3; c++){
        tmp[i][j][c] = pnm_get_component(ims, i, j, c);
      }
      // rgb to lms
      float tmp_lms[3];
      float tmp_lab[3];
      for(int mat_line = 0; mat_line < D; mat_line++){
        tmp_lms[mat_line] = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lms[mat_line] += RGB2LMS[mat_line][mat_col] * tmp[i][j][mat_col];
        }
        // lms to log lms
        tmp[i][j][mat_line] = log(tmp_lms[mat_line]);
        // lab
        tmp_lab[mat_line] = 0.0;
        for(int mat_col = 0; mat_col < D; mat_col++){
          tmp_lab[mat_line] += LOGLMS2LAB[mat_line][mat_col] * tmp[i][j][mat_col];
        }
        tmp[i][j][mat_line] = tmp_lab[mat_line];

        res[i*j + mat_line] = tmp[i][j][mat_line];
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

  // Result image
  pnm res = pnm_new(transfer_cols, transfer_rows, PnmRawPpm);
  // RGB List (transfer image)
  /* Peut-etre a modifier, je sais pas si tmp est bien comme ceci, mais
  l'idee est de faire un tableau 3D avec i = row, j = col et c = color */
  float *data_source = malloc(source_rows * source_cols * 3 * sizeof(float));
  float *data_transfer = malloc(transfer_rows * transfer_cols * 3 * sizeof(float));
  // RGB to L-alpha-beta
  printf("Computing l-alpha-beta for source image...\n");
  rgb_to_lab(source, data_source);
  printf("%f, %f, %f\n", data_source[0], data_source[1], data_source[2]);
  printf("%f, %f, %f\n", data_source[3], data_source[4], data_source[5]);
  printf("Source done. Now computing for transfer image...\n");
  rgb_to_lab(transfer, data_transfer);
  printf("%f, %f, %f\n", data_transfer[0], data_transfer[1], data_transfer[2]);
  printf("%f, %f, %f\n", data_transfer[3], data_transfer[4], data_transfer[5]);

  // <l-alpha-beta>
  float source_lab_avg[3] = {0.0, 0.0, 0.0};
  float transfer_lab_avg[3] = {0.0, 0.0, 0.0};
  for(int i = 0; i < transfer_rows; i++){
    for (int j = 0; j < transfer_cols; j++){
      for(int c = 0; c < 3; c++){
        transfer_lab_avg[c] += data_transfer[i*j + j + c];
        printf("New value for transfer sum : %f\n", transfer_lab_avg[c]);
      }
    }
  }
  for(int i = 0; i < source_rows; i++){
    for (int j = 0; j < source_cols; j++){
      for(int c = 0; c < 3; c++){
        source_lab_avg[c] += data_source[i*j + j + c];
      }
    }
  }
  for(int c = 0; c < 3; c++){
    source_lab_avg[c] /= source_cols* source_rows;
    transfer_lab_avg[c] /= transfer_cols* transfer_rows;
    printf("Computed average for :\nSource : %f\nTransfer : %f\n", source_lab_avg[c], transfer_lab_avg[c]);
  }
  /* TODO */
  // Calculer l*, a* et b*
  // Calculer l', a' et b'
  // Rajouter aux resultats précedents la moyenne de l', a' et b'
  // Conversion l'a'b' -> log LMS
  // Conversion log LMS -> LMS
  // Conversion LMS -> RGB

  pnm_save(res, PnmRawPpm, imd);
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
