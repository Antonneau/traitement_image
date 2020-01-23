#include <stdlib.h>
#include <stdio.h>

#include <bcl.h>


void 
process(int n, char *ims, char *imd)
{
  (void) n;
  (void) ims;
  (void) imd;  
}

void
usage (char *s){
  fprintf(stderr, "Usage: %s <n> <ims> <imd>\n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 3
int
main(int argc, char *argv[]){
  if (argc != PARAM+1) usage(argv[0]);
  process(0, NULL, NULL);
  return EXIT_SUCCESS;
}
