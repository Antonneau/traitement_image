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
  int data_rgb[transfer_rows * transfer_cols * 3];
  int (*rgb)[transfer_rows][3] = (int (*)[transfer_rows][3])data_rgb;
  for(int i = 0; i < transfer_rows; i++){
    for (int j = 0; j < transfer_cols; j++){
      for(int c = 0; c < 3; c++){
        rgb[i][j][c] = pnm_get_component(transfer, i, j, c);
        printf("Printing at index [%d][%d][%d]. Value : %d\n", i, j, c, rgb[i][j][c]);
      }
    }
  }

  int data_lms[transfer_rows * transfer_cols * 3];
  int (*lms)[transfer_rows][3] = (int (*)[transfer_rows][3])data_lms;
  /* TODO */
  // Convertion RGB -> LMS
  // Log LMS
  // Conversion LMS -> Lab
  // Calculer l*, a* et b*
  // Calculer l', a' et b'
  // Rajouter aux resultats précedents la moyenne de l', a' et b'
  // Conversion l'a'b' -> log LMS
  // Conversion log LMS -> LMS
  // Conversion LMS -> RGB

  pnm_save(res, PnmRawPpm, imd);
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
