#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<num> <ims> <imd>\n <num> is supposed to be between 0 and 2\n");
  exit(EXIT_FAILURE);
}

#define PARAM 3
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1 || atoi(argv[1]) < 0 || atoi(argv[1]) > 2) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  int chanToExtract = atoi(argv[1]);
  pnm ims = pnm_load(argv[2]);
  int cols = pnm_get_width(ims);
  int rows = pnm_get_height(ims);

  pnm imd = pnm_new(cols, rows, PnmRawPpm);

  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      unsigned short component = pnm_get_component(ims, i, j, chanToExtract);
      for (int chan = 0; chan <= 2; chan++){
        pnm_set_component(imd, i, j, chan, component);
        
      } 
    }
  }
  pnm_save(imd, PnmRawPpm, argv[3]);

  pnm_free(ims);
  pnm_free(imd);
  return EXIT_SUCCESS;
}