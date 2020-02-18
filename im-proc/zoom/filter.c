#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bcl.h>

#define NB_PARAMS 4

/**
 * @brief Rotates an image at 90 degres 
 * @param w the image width 
 * @param h the image height
 * @param ims the image source
 * @param imd the rotated image
 */
void
rotate_image(int w, int h, pnm ims, pnm imd){
    for(int i = 0; i < h; i++){
        for(int j = 0; j < w; j++){
            for(int c = 0; c < 3; c++){
                unsigned short comp = pnm_get_component(ims, j, i, c);
                pnm_set_component(imd, i, h-j-1, c, comp);
            }
        }
    }
    // TO DO
    // Donner la possibilité de le tourner dans un sens et dans l'autre
}

/**
 * @brief Computes the filter filter_num and saves it in a new image
 * @param 
 * @param 
 * @param 
 * @param 
 * @param 
 * @param 
 */
void 
compute_filter(int w, int h, int factor, int filter_num, pnm ims, pnm imd){
    for(int i = 0; i < h * factor; i++){
        for(int j = 0; j < w * factor-1; j++){
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
                    switch(filter_num){
                        // Box
                        case 0:
                            if(x >= -0.5 && x < 0.5){
                                sum[s] += pnm_get_component(ims, k, i/factor, s);
                            }
                            break;
                        // Tent
                        case 1:
                            break;
                        // Bell
                        case 2:
                            break;
                        // Mitch
                        case 3:
                            break;
                    }
                }
                // Putting each color in the new image
                for(int s = 0; s < 3; s++){
                    pnm_set_component(imd, col, i+x, s, sum[s]);
                }
            }
        }
    }
}

void
reconstruct_image(int factor, pnm col_ims, pnm line_ims, pnm imd)
{
    for(int i = 0; i < pnm_get_width(imd); i++){
        for(int j = 0; j < pnm_get_height(imd); j++){
            for(int c = 0; c < 3; c++){
                unsigned short sum_comp = 0;
                sum_comp = pnm_get_component(col_ims, j, i, c) + pnm_get_component(line_ims, j, i, c);
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

    pnm modified_imd = pnm_new(cols*factor, rows*factor, PnmRawPpm);

    // RETOURNER L'IMAGE POUR AUSSI FAIRE L'OPERATION SUR LE LIGNES
    // CAR L'OPERATION NE SE FAIT QU'EN COLONNES
    compute_filter(cols, rows, factor, filter_num, ims, modified_imd);

    pnm new_ims = pnm_new(cols, rows, PnmRawPpm);
    pnm rotated_imd = pnm_new(cols*factor, rows*factor, PnmRawPpm);
    rotate_image(cols, rows, ims, new_ims);
    compute_filter(cols, rows, factor, filter_num, new_ims, rotated_imd);
    
    pnm imd = pnm_new(cols*factor, rows*factor, PnmRawPpm);
    reconstruct_image(factor, modified_imd, rotated_imd, imd);

    pnm_save(imd, PnmRawPpm, argv[4]);
    pnm_save(modified_imd, PnmRawPpm, "test_modif");
    pnm_save(rotated_imd, PnmRawPpm, "test_rotated");

    pnm_free(imd);
    pnm_free(ims);
    pnm_free(new_ims);
    pnm_free(modified_imd);
    pnm_free(rotated_imd);

    return EXIT_SUCCESS;
}

