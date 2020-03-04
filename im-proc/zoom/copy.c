#include <stdlib.h>
#include <stdio.h>
#include <bcl.h>

#define NB_PARAMS 3

/**
 * @brief Copy the same pixel depending on the factor.
 * @param ims the image to get the pixel
 * @param imd the image to write
 * @param col the index of the column write
 * @param row the index of the row to write
 * @param factor the zoom factor
 */
void
copy_pixels(pnm ims, pnm imd, int col, int row, int factor)
{
    unsigned short c[3];
    for(int k = 0; k < 3; k++) 
        c[k] = pnm_get_component(ims, row, col, k);
    for(int i = 0; i < factor; i++)
        for(int j = 0; j < factor; j++)
            for(int k=0; k < 3; k++)
                pnm_set_component(imd, factor*row+i, factor*col+j, k, c[k]);
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
    
    pnm ims = pnm_load(argv[2]);
    int w = pnm_get_width(ims);
    int h = pnm_get_height(ims);
    int factor = atoi(argv[1]);

    pnm imd = pnm_new(w*factor, h*factor, PnmRawPpm);
    for(int i = 0; i < w; i++)
        for(int j = 0; j < h; j++)
            copy_pixels(ims, imd, i, j, factor);


    pnm_save(imd, PnmRawPpm, argv[3]);

    pnm_free(imd);
    pnm_free(ims);

    return EXIT_SUCCESS;
}

