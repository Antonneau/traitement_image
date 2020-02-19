#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <bcl.h>

/**
 * @brief Rotates an image at 90 degrees
 * @param w the image width 
 * @param h the image height
 * @param ims the image source
 * @param imd the rotated image
 * @param revert if the user wants a -90 degree rotation
 */
void
rotate_image(int w, int h, pnm ims, pnm imd, bool revert){
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            for(int c = 0; c < 3; c++){
                unsigned short comp = pnm_get_component(ims, j, i, c);
                if(!revert)
                    pnm_set_component(imd, i, h-j-1, c, comp);
                else
                    pnm_set_component(imd, h-i-1, j, c, comp);
            }
        }
    }
}

/**
 * @brief Computes the filter filter_num and saves it in a new image
 * @param w The width of the original picture
 * @param h The height of the original picture
 * @param factor The size factor
 * @param filter_num Index of the filter.
 *                   0 = box
 *                   1 = tent
 *                   2 = bell
 *                   3 = mitch
 * @param ims The source image
 * @param imd The destination image
 */
void 
compute_filter(int w, int h, int factor, int filter_num, pnm ims, pnm imd){
    for(int i = 0; i < h * factor; i++){
    //printf("Tour de boucle...\n");
        for(int j = 0; j < (w * factor)-1; j++){
            float col = j / factor;
            float WF = (filter_num + 1) / 2;
            // Getting left
            float left = col - WF;
            if(left < 0){
                left = 0;
            }
            // Getting right
            float right = col + WF;
            if(right > w*factor){
                right = w*factor;
            }
            // Initialising color sum
            int sum[3];
            for(int s = 0; s < 3; s++){
                sum[s] = 0;
            }
            // Iterating over the scaled image
            for(int k = left; k <= right; k ++){
                float x = k - col;
                // For each color
                for(int s = 0; s < 3; s++){
                    float filter = 0;
                    switch(filter_num){
                        // Box
                        case 0:
                            if(x >= -0.5 && x < 0.5)
                                filter = 1.0;
                            break;
                        // Tent
                        case 1:
                            if(x >= -1 && x <= 1)
                                filter = 1 - abs(x);
                            break;
                        // Bell
                        case 2:
                            if(abs(x) <= 0.5)
                                filter = -pow(x,2) + 0.75;
                            else if(abs(x) > 0.5 && abs(x) <= 1.5)
                                filter = 0.5 * pow((abs(x)-1.5), 2);
                            break;
                        // Mitch
                        case 3:
                            if(x >= -1 && x <= 1)
                                filter = ((7/6) * pow(abs(x), 3)) - (2*pow(x, 2)) + (8/9);
                            else if ((x >= -2 && x <= -1) || (x >= 1 && x <= 2))
                                filter = -((7/18) * pow(abs(x), 3)) + (2*pow(x, 2)) - ((10/3) * abs(x)) + (16/9);
                            break;
                    }
                    sum[s] += pnm_get_component(ims, k, i/factor, s) * filter;
                }
                // Putting each color in the new image
                for(int s = 0; s < 3; s++){
                    int row = i + x;
                    if(row < 0)
                        row = 0;
                    /*if(row > h*factor){
                        row = h*factor;
                    }*/
                    int column = col*factor;
                    //printf("%d, while width is %d\n", column, w*factor);
                    if(column >= w*factor){
                        column = w*(factor-1);
                    }
                    // TO DO
                    // Correct this for the tent filter
                    pnm_set_component(imd, column, row, s, sum[s]);
                }
            }
        }
    }
}

/**
 * @brief Reconstruct the zoomed image depending on two pictures : the picture with the filter computed on its columns and on its lines.
 * @param factor the size factor 
 * @param col_ims The filtered image on columns
 * @param line_ims The filtered image on lines
 * @param imd the reconstructed image
 */
void
reconstruct_image(int factor, pnm col_ims, pnm line_ims, pnm imd)
{
    for(int i = 0; i < pnm_get_height(imd); i++){
        for(int j = 0; j < pnm_get_width(imd); j++){
            for(int c = 0; c < 3; c++){
                unsigned short sum_comp = 0;
                sum_comp = pnm_get_component(col_ims, factor*(j/factor), factor*(i/factor), c) 
                         + pnm_get_component(line_ims, factor*(j/factor), factor*(i/factor), c);
                pnm_set_component(imd, j, i, c, sum_comp/2);
            }
        }
    }
    (void)factor;
}

void
usage(const char *s)
{
    fprintf(stderr, "Usage : %s <factor> <filter-name> <ims> <imd>\n", s);
    exit(EXIT_FAILURE);
}

#define NB_PARAMS 4

int 
main(int argc, char** argv)
{
    if(argc != NB_PARAMS+1)
        usage(argv[0]);

    // Checking if the filter name exists
    int filter_num = -1;
    if(strcmp(argv[2], "box") == 0)
        filter_num = 0;
    else if(strcmp(argv[2], "tent") == 0)
        filter_num = 1;
    else if(strcmp(argv[2], "bell") == 0)
        filter_num = 2;
    else if(strcmp(argv[2], "mitch") == 0)
        filter_num = 3;
    else 
        usage(argv[0]);

    pnm ims = pnm_load(argv[3]);
    int cols = pnm_get_width(ims);
    int rows = pnm_get_height(ims);
    int factor = atoi(argv[1]);

    // First computation of the filter : on columns
    pnm modified_imd = pnm_new(cols*factor, rows*factor, PnmRawPpm);
    compute_filter(cols, rows, factor, filter_num, ims, modified_imd);

    // Second computation of the filter : on lines (we rotate the original picture)
    pnm rotated_imd = pnm_new(cols*factor, rows*factor, PnmRawPpm);
    pnm rotated_ims = pnm_new(cols, rows, PnmRawPpm);
    rotate_image(cols, rows, ims, rotated_ims, false);
    compute_filter(cols, rows, factor, filter_num, rotated_ims, rotated_imd);
    
    // Reconstruction of the image
    pnm rerotated_imd = pnm_new(cols*factor, rows*factor, PnmRawPpm);
    rotate_image(cols*factor, rows*factor, rotated_imd, rerotated_imd, true);
    pnm imd = pnm_new(cols*factor, rows*factor, PnmRawPpm);
    reconstruct_image(factor, modified_imd, rerotated_imd, imd);

    pnm_save(imd, PnmRawPpm, argv[4]);

    pnm_free(ims);
    pnm_free(rotated_ims);
    pnm_free(modified_imd);
    pnm_free(rotated_imd);
    pnm_free(rerotated_imd);
    pnm_free(imd);

    return EXIT_SUCCESS;
}

