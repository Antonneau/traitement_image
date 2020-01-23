#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>

void 
usage (char *s)
{
  fprintf(stderr,"Usage: %s %s", s, "<i> <j> <rows> <cols> <ims> <imd>\n");
  exit(EXIT_FAILURE);
}

#define PARAM 6
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

  int base_row = atoi(argv[1]);
  int base_col = atoi(argv[2]);
  int rows = atoi(argv[3]);
  int cols = atoi(argv[4]);

  pnm ims = pnm_load(argv[5]);
  pnm imd = pnm_new(rows, cols, PnmRawPpm);

  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      for (int color = 0; color < 3; color++){
        unsigned short component = pnm_get_component(ims, i + base_row, j + base_col, color);
        pnm_set_component(imd, i, j, color, component);
      }
    }
  }
  pnm_save(imd, PnmRawPpm, argv[6]);

  pnm_free(ims);
  pnm_free(imd);
  return EXIT_SUCCESS;
}
