/**
 * @file test-fft.c
 * @brief test the behaviors of functions in fft module
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fft.h>
#include <assert.h>
#include <math.h>

/**
 * @brief generates a gray-scale image
 * @param cols the number of columns of the input image
 * @param rows the number of lines of the input image
 * @param ims the input image
 * @param imd the gray-scale image
 * @return the gray-scale input image
 */
void
generate_gray_image(int cols, int rows, pnm ims, unsigned short *imd)
{
  // Constructing the gray image
  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      unsigned short redComponent = pnm_get_component(ims, i, j, 0);
      unsigned short greenComponent = pnm_get_component(ims, i, j, 1);
      unsigned short blueComponent = pnm_get_component(ims, i, j, 2);
      unsigned short component = (redComponent + greenComponent + blueComponent) / 3;
      imd[i + (rows*j)] = component; 
    }
  }
}

/**
 * @brief Saves an image
 * @param name the image name
 * @param prefix the prefix of the image name
 * @param imd the image to save
 * @return The offset (index where the filename is in the string)
 */
void
save_image(char *name, char* prefix, pnm imd){
  char* nameimg = base_name(name);
  char fileName[strlen(prefix)+strlen(nameimg)];
  sprintf(fileName,"%s%s",prefix,nameimg);
  pnm_save(imd, PnmRawPpm, fileName);
}

/**
 * @brief Reconstructs the gray-scale image from its list
 * @param cols the number of columns of the input image
 * @param rows the number of lines of the input image
 * @param ims the input list of gray pixels
 * @param imd the gray-scale image
 * @return the reconstructed gray-scale input image
 */
void
set_comp(int cols, int rows, unsigned short* ims, pnm imd)
{
  for (int j = 0; j < cols; j++)
    for (int i = 0; i < rows; i++)
      for (int chan = 0; chan <= 2; chan++)
        pnm_set_component(imd, i, j, chan, ims[i + (rows*j)]);
}

/**
 * @brief test the forward and backward functions
 * @param char* name, the input image file name
 */
void
test_forward_backward(char* name)
{
  fprintf(stderr, "test_forward_backward: ");

  pnm img = pnm_load(name);
  int cols = pnm_get_width(img);
  int rows = pnm_get_height(img);

  unsigned short *g_img = malloc(rows*cols*sizeof(unsigned short));
  generate_gray_image(cols, rows, img, g_img);
  pnm_free(img);

  fftw_complex *comp = forward(rows, cols, g_img);
  unsigned short *new_g_img = backward(rows, cols, comp);

  free(g_img);

  pnm new_image = pnm_new(cols, rows, PnmRawPpm);
  set_comp(cols,rows,new_g_img,new_image);
  save_image(name,"FB-", new_image);
  
  free(new_g_img);
  free(comp);
  pnm_free(new_image);
  
  fprintf(stderr, "OK\n");
}

/**
 * @brief test image reconstruction from of magnitude and phase spectrum
 * @param char *name: the input image file name
 */
void
test_reconstruction(char* name)
{
  fprintf(stderr, "test_reconstruction: ");
  pnm img = pnm_load(name);
  int cols = pnm_get_width(img);
  int rows = pnm_get_height(img);

  unsigned short *g_img = malloc(rows*cols*sizeof(unsigned short));
  generate_gray_image(cols, rows, img, g_img);
  pnm_free(img);

  fftw_complex *comp = forward(rows, cols, g_img);
  
  float* as = malloc(cols*rows*sizeof(float));
  float* ps = malloc(cols*rows*sizeof(float));
  
  freq2spectra(rows,cols,comp,as,ps);
  spectra2freq(rows,cols,as,ps,comp);

  unsigned short *new_g_img = backward(rows, cols, comp);

  pnm new_image = pnm_new(cols, rows, PnmRawPpm);
  set_comp(cols,rows,new_g_img,new_image);

  save_image(name,"FB-ASPS-", new_image);

  pnm_free(new_image);
  free(comp);
  free(as);
  free(ps);
  free(g_img);
  fprintf(stderr, "OK\n");
}

/**
 * @brief Recenter the frequency image, in order to put the low frequencies at the center at the image.
 * @param cols the number of columns of the input image
 * @param rows the number of lines of the input image
 * @param tab the input image
 * @return the gray-scale input image
 */
float* decenter(int cols, int rows, float *tab){
  float *tmp = malloc(cols*rows*sizeof(float));
  for (int j = 0; j < cols/2; j++){
    for (int i = 0; i < rows/2; i++){
      tmp[i       + rows*j          ] = tab[i+rows/2+ rows*(j+cols/2) ];
      tmp[i+rows/2+ rows*j          ] = tab[i       + rows*(j+cols/2) ];
      tmp[i       + rows*(j+cols/2) ] = tab[i+rows/2+ rows*j          ];
      tmp[i+rows/2+ rows*(j+cols/2) ] = tab[i       + rows*j          ];
      }
  }
  free(tab);
  return tmp;
}

/**
 * @brief test construction of magnitude and phase images in ppm files
 * @param char* name, the input image file name
 */
void
test_display(char* name)
{
  fprintf(stderr, "test_display: ");
  
  pnm img = pnm_load(name);
  int cols = pnm_get_width(img);
  int rows = pnm_get_height(img);
  int size = cols*rows;

  unsigned short *g_img = malloc(rows*cols*sizeof(unsigned short));
  generate_gray_image(cols, rows, img, g_img);
  pnm_free(img);

  fftw_complex *comp = forward(rows, cols, g_img);
  
  float* as = malloc(cols*rows*sizeof(float));
  float* ps = malloc(cols*rows*sizeof(float));
  freq2spectra(rows,cols,comp,as,ps);
  
  float amax= as[0];

  // Getting the maximum amp
  for(int i=1; i<size; i++)
    if(as[i]>amax) 
      amax=as[i];

  // Centering the frequencies
  float* ass = decenter(cols, rows, as);
  float* pss = decenter(cols, rows, ps);

  pnm new_image_amp = pnm_new(cols, rows, PnmRawPpm);
  pnm new_image_phs = pnm_new(cols, rows, PnmRawPpm);
  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      for (int chan = 0; chan <= 2; chan++){
        // Normalizing the values and pass them to the images
        short valas = (short) (pow(fabs(ass[i + (rows*j)])/amax,0.2)*255);
        pnm_set_component(new_image_amp, i, j, chan, valas);
        pnm_set_component(new_image_phs, i, j, chan, (short) (pss[i + (rows*j)]));
      } 
    }
  }

  save_image(name,"AS-", new_image_amp);
  save_image(name,"PS-", new_image_phs);
  
  pnm_free(new_image_amp);
  pnm_free(new_image_phs);
  free(comp);
  free(as);
  free(ps);
  free(ass);
  free(pss);
  free(g_img);

  fprintf(stderr, "OK\n");
}

/**
 * @brief test the modification of magnitude by adding a periodic functions
          on both vertical and horizontal axis, and 
 *        construct output images
 * @param char* name, the input image file name
 */
void

test_add_frequencies(char* name)
{
  fprintf(stderr, "test_add_frequencies: ");
    
  pnm img = pnm_load(name);
  int cols = pnm_get_width(img);
  int rows = pnm_get_height(img);
  int size = cols*rows;

  unsigned short *g_img = malloc(rows*cols*sizeof(unsigned short));
  generate_gray_image(cols, rows, img, g_img);
  pnm_free(img);

  fftw_complex *comp = forward(rows, cols, g_img);
  
  float* as = malloc(cols*rows*sizeof(float));
  float* ps = malloc(cols*rows*sizeof(float));
  freq2spectra(rows,cols,comp,as,ps);
  
  float amax= as[0];

  for(int i=1; i<size; i++)
    if(as[i]>amax) 
      amax=as[i];

  float* new_as = malloc(cols*rows*sizeof(float));
  for(int i = 0; i < size; i++){
     // Asin(2pif + phi);
     // The new amplitude list, using a sinusoidal function
     new_as[i] = (as[i]*0.25*amax)*sin(2*pi*8 + ps[i]);
  }

  float* new_as_cen = decenter(cols, rows, new_as);

  pnm new_image_amp = pnm_new(cols, rows, PnmRawPpm);
  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      for (int chan = 0; chan <= 2; chan++){
        short valas = (short) (pow(fabs(new_as_cen[i + (rows*j)])/amax,0.2)*255);
        pnm_set_component(new_image_amp, i, j, chan, valas);
      } 
    }
  }

  spectra2freq(rows, cols, new_as, ps, comp);
  g_img = backward(rows, cols, comp);
  pnm new_image = pnm_new(cols, rows, PnmRawPpm);
  set_comp(cols, rows, g_img, new_image);

  // The new gray-scaled image with the frequencies modification
  save_image(name, "FREQ-", new_image);
  // The frequency image.
  save_image(name, "FAS-", new_image_amp);
  
  pnm_free(new_image_amp);
  pnm_free(new_image);
  free(comp);
  free(as);
  free(ps);
  free(new_as);
  free(new_as_cen);
  free(g_img);

  fprintf(stderr, "OK\n");
}

void
run(char* name)
{
  test_forward_backward(name);
  test_reconstruction(name);
  test_display(name);
  test_add_frequencies(name);
}

void 
usage(const char *s)
{
  fprintf(stderr, "Usage: %s <ims> \n", s);
  exit(EXIT_FAILURE);
}

#define PARAM 1
int 
main(int argc, char *argv[])
{
  if (argc != PARAM+1) usage(argv[0]);
  run(argv[1]);
  return EXIT_SUCCESS;
}
