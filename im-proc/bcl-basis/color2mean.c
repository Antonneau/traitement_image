#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 2
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  pnm ims = pnm_load(argv[1]);
  int cols = pnm_get_width(ims);
  int rows = pnm_get_height(ims);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);

  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      unsigned short redComponent = pnm_get_component(ims, i, j, 0);
      unsigned short greenComponent = pnm_get_component(ims, i, j, 1);
      unsigned short blueComponent = pnm_get_component(ims, i, j, 2);
      unsigned short component = (redComponent + greenComponent + blueComponent) / 3;
      for (int chan = 0; chan <= 2; chan++){
        pnm_set_component(imd, i, j, chan, component);
      } 
    }
  }
  pnm_save(imd, PnmRawPpm, argv[2]);

  pnm_free(ims);
  pnm_free(imd);
  return EXIT_SUCCESS;
}