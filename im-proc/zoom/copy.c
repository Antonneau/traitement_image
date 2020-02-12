#include <float.h>
#include <stdlib.h>
#include <math.h>
#include <bcl.h>

#define NB_PARAMS 3

/**
 * @brief Puts the same pixel depending on the factor.
 * @param ims the image to get the pixel
 * @param imd the image to write
 * @param col the index of the column write
 * @param row the index of the row to write
 * @param factor the zoom factor
 */
void
put_pixels(pnm ims, pnm imd, int col, int row, int factor)
{
    //TODO
}

void
usage(const char *s)
{
    fprintf(stderr, "Usage : %s <factor> <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

int 
main(int argc, char* argv){

    if(argc != NB_PARAMS+1)
        usage(argv[0]);
    
    pnm ims = pnm_load(argv[2]);
    int cols = pnm_get_width(ims);
    int rows = pnm_get_height(img);
    int factor = atoi(argv[1]);

    pnm imd = pnm_new(cols*factor, rows*factor, PnmRawPpm);

    for(int i = 0; i < rows, i++){
        for(int j = 0; j < cols; i++){
            put_pixels(ims, imd, j, i, factor);
        }
    }

    pnm_save(imd, PnmRawPpm, argv[3]);

    pnm_free(imd)
    pnm_free(ims)

    return EXIT_SUCCESS;
}

