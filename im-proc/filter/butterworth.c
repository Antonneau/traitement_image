#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h> 

#include <bcl.h>
#include <fft.h>

float
lowpass(int u, int v, int d0, int n, int w, int u0, int v0){
  (void)u;
  (void)v;
  (void)d0;
  (void)n;
  (void)w;
  (void)u0;
  (void)v0;  
  return 0.0;
}

float
highpass(int u, int v, int d0, int n, int w, int u0, int v0){
  (void)u;
  (void)v;
  (void)d0;
  (void)n;
  (void)w;
  (void)u0;
  (void)v0;  
  return 0.0;
}

float
bandreject(int u, int v, int d0, int n, int w, int u0, int v0){
  (void)u;
  (void)v;
  (void)d0;
  (void)n;
  (void)w;
  (void)u0;
  (void)v0;  
  return 0.0;
}

float
bandpass(int u, int v, int d0, int n, int w, int u0, int v0){
  (void)u;
  (void)v;
  (void)d0;
  (void)n;
  (void)w;
  (void)u0;
  (void)v0;  
  return 0.0;
}

float
notch(int u, int v, int d0, int n, int w, int u0, int v0){
  (void)u;
  (void)v;
  (void)d0;
  (void)n;
  (void)w;
  (void)u0;
  (void)v0;  
  return 0.0;
}

void 
process(char* inp, char* out, 
	int d0, int nx2, int ww, int u0, int v0,
	float (*apply)(int, int, int, int, int, int, int))  
{
  (void)inp;
  (void)out;
  (void)d0;
  (void)nx2;
  (void)ww;
  (void)u0;
  (void)v0;
  (void)apply;  
}

void usage (char *s){
  fprintf(stderr, "Usage: %s <ims> <imd> <filter> ", s);
  fprintf(stderr, "<d0> <n> <w> <u0> <v0>\n");
  exit(EXIT_FAILURE);
}

#define param 8
int
main(int argc, char *argv[]){
  if (argc != param+1) usage(argv[0]);
  return EXIT_SUCCESS;
}
