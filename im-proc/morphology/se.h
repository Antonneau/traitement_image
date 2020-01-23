#include <bcl.h>

//enum {SQUARE, DIAMOND, DISK, LINE_V, DIAG_R, LINE_H, DIAG_L, CROSS, PLUS};
/**
 * @brief  generate a structuring element of a given shape and halfsize 
 * @param  shape: the structing element shape umber, can be defined with the 
 *         above enum
 * @param  halfsize: the structuring element halfsize
 * @return the structuring element of (2*halfsize+1)^2 size as a pnm object
 */
pnm 
se(int shape, int halfsize); 

