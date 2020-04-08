#include <stdlib.h>
#include <stdio.h>

#include <morphology.h>
#include <se.h>

void
usage(char* s)
{
  fprintf(stderr,"%s <se> <hs> <imd>\n",s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int
main(int argc, char* argv[])
{
  if(argc != PARAM+1) usage(argv[0]);
  pnm imd = se(atoi(argv[1]), atoi(argv[2]));
  pnm_save(imd, PnmRawPpm, argv[3]);
  pnm_free(imd);
  return EXIT_SUCCESS;
}
