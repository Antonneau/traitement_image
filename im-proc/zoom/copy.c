#include <float.h>
#include <stdlib.h>
#include <math.h>
#include <bcl.h>

#define NB_PARAMS 3

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

            for(int f = 0; f < factor; f++){
                for(int chan = 0; chan <= 2; chan++){
                    

                }
            }
            
        }
    }
}

