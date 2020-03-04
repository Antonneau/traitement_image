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
                unsigned short comp = pnm_get_component(ims, i, j, c);
                if(!revert)
                    pnm_set_component(imd, w-j-1, i, c, comp);
                else
                    pnm_set_component(imd, j, h-i-1, c, comp);
            }
        }
    }
}

float filter_value(float x, int filter_num){
    float filter=0.0;
    switch(filter_num){
        // Box
        case 0:
            if(x >= -0.5 && x < 0.5)
                filter = 1.0;
            break;
        // Tent
        case 1:
            if(x >= -1 && x <= 1)
                filter = 1.0 - fabs(x);
            break;
        // Bell
        case 2:
            if(fabs(x) <= 0.5)
                filter = -(x*x) + 0.75;
            else if(fabs(x) > 0.5 && fabs(x) <= 1.5)
                filter = 0.5 * (fabs(x)-1.5)*(fabs(x)-1.5);
            break;
        // Mitch
        case 3:
            if(x >= -1 && x <= 1)
                filter = ((7.0/6.0) * fabs(x)*fabs(x)*fabs(x)) - (2*x*x) + (8.0/9.0);
            else if ((x >= -2 && x <= -1) || (x >= 1 && x <= 2))
                filter = (-7.0/18.0) * fabs(x)*fabs(x)*fabs(x) + (2*x*x) - ((10.0/3.0) * fabs(x)) + (16.0/9.0);
            break;
    }
    return filter;
    
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
    for(int i = 0; i < h; i++){
        for(int j = 0; j < (w * factor); j++){
            float col = (float) j / (float) factor;
            float WF = (float) (filter_num + 1) / 2.0;
            // Getting left
            int left = floor(col - WF);
            if(left < 0)
                left = 0;
            // Getting right
            int right = floor(col + WF);
            // Initialising color sum
            float sum[3] = {0.0,0.0,0.0};
            // Iterating over the scaled image
            for(int k = left; k <= right; k ++){
                float x = k - col;
                float filter = filter_value(x,filter_num);
                // For each color
                for(int s = 0; s < 3; s++){
                    int tmp = k;
                    if (k >= w)
                        tmp = w-1;
                    sum[s] += pnm_get_component(ims, i, tmp, s) * filter;
                }
            }
            // Putting each color in the new image
            for(int s = 0; s < 3; s++){
                if(sum[s] < 0.0) sum[s] = 0.0;
                if(sum[s] >255.0) sum[s] = 255.0;
                pnm_set_component(imd, i, j, s, sum[s]);
            }
        }
    }
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
    pnm modified_imd = pnm_new(cols*factor, rows, PnmRawPpm);
    compute_filter(cols, rows, factor, filter_num, ims, modified_imd);
    
    // Second computation of the filter : on lines (we rotate the original picture)
    pnm rotated_imd = pnm_new(rows ,cols*factor , PnmRawPpm);
    rotate_image(cols*factor, rows, modified_imd, rotated_imd, false);
    pnm imd = pnm_new(rows*factor, cols*factor, PnmRawPpm);
    compute_filter(rows, cols*factor, factor, filter_num, rotated_imd, imd);

    // Reconstruction of the image
    pnm rerotated_imd = pnm_new(cols*factor, rows*factor, PnmRawPpm);
    rotate_image(rows*factor, cols*factor, imd, rerotated_imd, true);
    pnm_save(rerotated_imd, PnmRawPpm, argv[4]);

    pnm_free(ims);
    pnm_free(modified_imd);
    pnm_free(rerotated_imd);
    pnm_free(imd);

    return EXIT_SUCCESS;
}

