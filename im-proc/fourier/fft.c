#include <float.h>
#include <stdlib.h>
#include <math.h>

#include <fft.h>

fftw_complex *
forward(int rows, int cols, unsigned short* g_img)
{
  unsigned int size = rows*cols;
  fftw_complex *in = malloc(size*sizeof(fftw_complex));
  fftw_complex *out = malloc(size*sizeof(fftw_complex));  // A probablement changer
  if (in == NULL || out == NULL){
    fprintf(stderr, "Cannot allocate more memory (in forward)\n");
    exit(EXIT_FAILURE);
  } 
  // Initializing the complex image
  for(unsigned int i = 0; i < size; i++){
    fftw_complex c = g_img[i] + I*0; 
    in[i] = c;
  } 

  fftw_plan plan = fftw_plan_dft_2d(rows, cols, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);

  fftw_destroy_plan(plan);
  free(in);
  return out;
}

unsigned short *
backward(int rows, int cols, fftw_complex* freq_repr)
{
  unsigned int size = rows*cols;
  fftw_complex *out = malloc(size*sizeof(fftw_complex));
  unsigned short *img = malloc(size*sizeof(unsigned short));
  if (out == NULL || img == NULL){
    fprintf(stderr, "Cannot allocate more memory (in backward)\n");
    exit(EXIT_FAILURE);
  } 

  fftw_plan plan = fftw_plan_dft_2d(rows, cols, freq_repr, out, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(plan);
  fftw_destroy_plan(plan);

  for(unsigned int i = 0; i < size; i++){
    double real = creal(out[i])/(size);
    img[i] = (unsigned short)real; 
  } 

  free(out);
  return img;
}

void
freq2spectra(int rows, int cols, fftw_complex* freq_repr, float* as, float* ps) 
{
  (void)rows;
  (void)cols;
  (void)freq_repr;
  (void)as;
  (void)ps;
}

void 
spectra2freq(int rows, int cols, float* as, float* ps, fftw_complex* freq_repr)
{
  (void)rows;
  (void)cols;
  (void)as;
  (void)ps;
  (void)freq_repr;
}
