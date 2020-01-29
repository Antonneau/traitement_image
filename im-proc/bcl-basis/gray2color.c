#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<ims0> <ims1> <ims2> <imd>\n");
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

  pnm redChannel = pnm_load(argv[1]);
  pnm greenChannel = pnm_load(argv[2]);
  pnm blueChannel = pnm_load(argv[3]);
  int cols = pnm_get_width(redChannel);
  int rows = pnm_get_height(redChannel);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);

  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      for (int chan = 0; chan <= 2; chan++){
        unsigned short component;
        switch(chan){
          case 0:
            component = pnm_get_component(redChannel, i, j, chan);
            break;
          case 1:
            component = pnm_get_component(greenChannel, i, j, chan);
            break;
          case 2:
            component = pnm_get_component(blueChannel, i, j, chan);
            break;
        }
        pnm_set_component(imd, i, j, chan, component);
      } 
    }
  }
  pnm_save(imd, PnmRawPpm, argv[4]);

  pnm_free(redChannel);
  pnm_free(greenChannel);
  pnm_free(blueChannel);
  pnm_free(imd);
  return EXIT_SUCCESS;
}