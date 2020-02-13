#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <fft.h> 

#define NB_PARAMS 3

/**
 * @brief Reconstructs the gray-scale image from its list
 * @param cols the number of columns of the input image
 * @param rows the number of lines of the input image
 * @param ims the input list of gray pixels
 * @param imd the gray-scale image to compute
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
 * @brief Computes a gray-scaled image from an input image
 * @param cols the number of columns of the input image
 * @param rows the number of rows of the input image
 * @param ims the input image
 * @param imd the new gray-scaled image
 **/
void
generate_gray_image(int cols, int rows, pnm ims, unsigned short *imd)
{
    // Constructing the gray image
    for (int j = 0; j < cols; j++)
        for (int i = 0; i < rows; i++){
            unsigned short component = 0;
            for (int c = 0; c < 3; c++)
                component += pnm_get_component(ims, i, j, c);
            imd[i + (rows*j)] = component/3; 
        }
}

/**
 * @brief Computes a gray-scaled image from an input image
 * @param in the number of columns of the input image
 * @param out the number of rows of the input image
 * @param cols the input image
 * @param rows the new gray-scaled image
 * @param factor the new gray-scaled image
 **/
void
resize_complex_array(fftw_complex *in, fftw_complex *out, int cols, int rows, int factor)
{
    // Initializing the zero array
    for (int j = 0; j < cols*factor; j++)
        for (int i = 0; i < rows*factor; i++)
            out[i+(rows*factor*j)]= 0+0*I;

    // Putting the input image at the center
    int starting_col = cols*factor/2-cols/2;
    int starting_row = rows*factor/2-rows/2;
    for (int j = 0; j < cols; j++)
        for (int i = 0; i < rows; i++)
            out[(starting_row+i)+(rows*factor*(j+starting_col))]= in[i + j*rows];
}


/**
 * @brief Computes the padding of an image, depending on a factor
 * @param ims the input image
 * @param imd the new image after padding
 * @param factor the scale factor
 **/
void
do_padding(pnm ims, pnm imd, int factor)
{
    int cols = pnm_get_width(ims);
    int rows = pnm_get_height(ims);

    unsigned short *g_img = malloc(rows*cols*sizeof(unsigned short));
    fftw_complex * comp = (fftw_complex *) malloc((rows*factor*cols*factor)*sizeof(fftw_complex));

    generate_gray_image(cols, rows, ims, g_img);

    fftw_complex *precomp = forward(rows, cols, g_img);

    resize_complex_array(precomp,comp,cols,rows,factor);

    unsigned short *new_g_img = backward(rows*factor, cols*factor, comp, cols*rows);

    set_comp(cols*factor,rows*factor, new_g_img, imd);

    free(g_img);
    free(new_g_img);
    free(precomp);
    free(comp);
}

void
usage(const char *s)
{
    fprintf(stderr, "Usage : %s <factor> <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

int 
main(int argc, char** argv)
{
    if(argc != NB_PARAMS+1)
        usage(argv[0]);
    
    int factor = atoi(argv[1]);
    pnm ims = pnm_load(argv[2]);
    pnm imd = pnm_new(pnm_get_width(ims) * factor, pnm_get_height(ims)*factor, PnmRawPpm);
    
    do_padding(ims,imd,factor);

    pnm_save(imd, PnmRawPpm, argv[3]);

    pnm_free(imd);
    pnm_free(ims);

    return EXIT_SUCCESS;
}