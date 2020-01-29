#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<min> <max> <ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 4
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  pnm ims = pnm_load(argv[3]);
  int cols = pnm_get_width(ims);
  int rows = pnm_get_height(ims);

  // min
  float min = atof(argv[1]);
  // max
  float max = atof(argv[2]);
  // Max(I)
  float maxValue = (float)pnm_maxval;
  // Min(I)
  float minValue = 0.0;
  pnm imd = pnm_new(cols, rows, PnmRawPpm);

  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      for (int chan = 0; chan <= 2; chan++){
        unsigned short component = pnm_get_component(ims, i, j, chan);
        float res1 = ((max - min) / (maxValue - minValue)) * (float)component;
        float res2 = (min * maxValue - max * minValue) / (maxValue - minValue);
        float res = res1 + res2;
        pnm_set_component(imd, i, j, chan, res);
      } 
    }
  }
  pnm_save(imd, PnmRawPpm, argv[4]);

  pnm_free(ims);
  pnm_free(imd);
  return EXIT_SUCCESS;
}