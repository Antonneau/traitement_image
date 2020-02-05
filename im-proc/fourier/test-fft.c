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
  // Constructing the gray image
  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      unsigned short redComponent = pnm_get_component(img, i, j, 0);
      unsigned short greenComponent = pnm_get_component(img, i, j, 1);
      unsigned short blueComponent = pnm_get_component(img, i, j, 2);
      unsigned short component = (redComponent + greenComponent + blueComponent) / 3;
      g_img[i + (rows*j)] = component; 
    }
  }
  pnm_free(img);

  fftw_complex *comp = forward(rows, cols, g_img);
  
  unsigned short *new_img = backward(rows, cols, comp);

  free(g_img);

  pnm new_image = pnm_new(cols, rows, PnmRawPpm);
  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      for (int chan = 0; chan <= 2; chan++){
        pnm_set_component(new_image, i, j, chan, new_img[i + (rows*j)]);
        //printf("%d\n", new_img[i + (rows*j)]);
      } 
    }
  }
  char* nameimg = name + 8;
  char *fileName = malloc((3+strlen(nameimg))*sizeof(char));
  sprintf(fileName,"FB-%s",nameimg);
  pnm_save(new_image, PnmRawPpm, fileName);
  free(fileName);
  pnm_free(new_image);
  free(new_img);
  free(comp);
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
  // Constructing the gray image
  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      unsigned short redComponent = pnm_get_component(img, i, j, 0);
      unsigned short greenComponent = pnm_get_component(img, i, j, 1);
      unsigned short blueComponent = pnm_get_component(img, i, j, 2);
      unsigned short component = (redComponent + greenComponent + blueComponent) / 3;
      g_img[i + (rows*j)] = component; 
    }
  }
  pnm_free(img);

  fftw_complex *comp = forward(rows, cols, g_img);
  float* as = malloc(cols*rows*sizeof(float));
  float* ps = malloc(cols*rows*sizeof(float));
  freq2spectra(rows,cols,comp,as,ps);
  spectra2freq( rows,cols,as,ps,comp);

  unsigned short *new_img = backward(rows, cols, comp);

  pnm new_image = pnm_new(cols, rows, PnmRawPpm);
  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      for (int chan = 0; chan <= 2; chan++){
        pnm_set_component(new_image, i, j, chan, new_img[i + (rows*j)]);
        //printf("%d\n", new_img[i + (rows*j)]);
      } 
    }
  }
  char* nameimg = name + 8;
  char *fileName = malloc((8+strlen(nameimg))*sizeof(char));
  sprintf(fileName,"FB-ASPS-%s",nameimg);
  pnm_save(new_image, PnmRawPpm, fileName);

  pnm_free(new_image);
  free(comp);
  free(as);
  free(ps);
  free(g_img);
  free(fileName);
  fprintf(stderr, "OK\n");
}

float* decenter(float *tab, int cols,int rows){
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
  // Constructing the gray image
  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      unsigned short redComponent = pnm_get_component(img, i, j, 0);
      unsigned short greenComponent = pnm_get_component(img, i, j, 1);
      unsigned short blueComponent = pnm_get_component(img, i, j, 2);
      unsigned short component = (redComponent + greenComponent + blueComponent) / 3;
      g_img[i + (rows*j)] = component; 
    }
  }
  pnm_free(img);

  fftw_complex *comp = forward(rows, cols, g_img);
  float* as = malloc(cols*rows*sizeof(float));
  float* ps = malloc(cols*rows*sizeof(float));
  freq2spectra(rows,cols,comp,as,ps);
  
  float amax= as[0];

  for(int i=1;i<size;i++)
    if(as[i]>amax) amax=as[i];

  float* ass = decenter(as,cols, rows);

  pnm new_image1 = pnm_new(cols, rows, PnmRawPpm);
  pnm new_image2 = pnm_new(cols, rows, PnmRawPpm);
  for (int j = 0; j < cols; j++){
    for (int i = 0; i < rows; i++){
      for (int chan = 0; chan <= 2; chan++){
        short valas = (short) (pow(fabs(ass[i + (rows*j)])/amax,0.1)*255);
        pnm_set_component(new_image1, i, j, chan, valas);
        pnm_set_component(new_image2, i, j, chan, (short) fabs((ps[i + (rows*j)]/rows)));
      } 
      printf("%lf\n", ps[i + (rows*j)]/size);
    }
  }
  char* nameimg = name + 8;
  char *fileName = malloc((3+strlen(nameimg))*sizeof(char));
  sprintf(fileName,"AS-%s",nameimg);
  pnm_save(new_image1, PnmRawPpm, fileName);

  sprintf(fileName,"PS-%s",nameimg);
  pnm_save(new_image2, PnmRawPpm, fileName);

  pnm_free(new_image1);
  pnm_free(new_image2);
  free(comp);
  free(fileName);
  free(ass);
  free(ps);
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
  (void)name;
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
