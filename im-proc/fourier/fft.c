#include <float.h>
#include <stdlib.h>
#include <math.h>

#include <fft.h>

fftw_complex *
forward(int rows, int cols, unsigned short* g_img)
{
  unsigned int size = rows*cols;
  fftw_complex *in = malloc(size*sizeof(fftw_complex));
  fftw_complex *out = malloc(size*sizeof(fftw_complex));  
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
  fftw_cleanup();
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
  fftw_cleanup();

  for(unsigned int i = 0; i < size; i++){
    // Set the normalizing value to the gray-scaled image
    double real = creal(out[i])/(size);
    if(real <0) img[i]=0;
    else if (real > 255) img[i] = 255;
    else img[i] = (unsigned short) real; 
  } 

  free(out);
  return img;
}

void
freq2spectra(int rows, int cols, fftw_complex* freq_repr, float* as, float* ps) 
{
  unsigned int size = rows*cols;

  for(unsigned int i = 0; i < size; i++){
    as[i] = cabs(freq_repr[i]);
    ps[i] = carg(freq_repr[i]); 
  } 
}

void 
spectra2freq(int rows, int cols, float* as, float* ps, fftw_complex* freq_repr)
{
  unsigned int size = rows*cols;

  for(unsigned int i = 0; i < size; i++){
    freq_repr[i]= as[i] * cexp(I * ps[i]);
  }
}
