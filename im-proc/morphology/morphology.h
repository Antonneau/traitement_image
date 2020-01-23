#ifndef __MORPHOLOGY_HH__
#define __MORPHOLOGY_HH__

/**
 *  @file  morphology.h
 *  @brief header for morphology.c that implements functions 
 *         for mathematical morphology
 *         image processing
 *	   A structuring element is viewed as a pnm binary image file : 0 for
 *	   the background and 255 for the shape.
 */
#include <stdlib.h>
#include <stdio.h>
#include <pnm.h>

/**
 * @brief  compute a morphological dilation or erosion on a grayscale image
 *         with a given structuring element. Dilation or erosion 
 *         processing depends on an order function  defined by the pointer pf
 * @param  shape: the structing element shape umber
 * @param  halfsize: the structuring element halfsize
 * @param  ims: the input image source to process
 * @param  imd: the destination image
 * @param  pf: a pointer on a ordering function
 */
void
process(int shape, 
	int halfsize, 
	pnm ims, 
	pnm imd, 
	void (*pf)(unsigned short*, unsigned short*));

/**
 * @brief  ordering function, if val is geater than max then update max
 * @param  val: a pointer to the input value(s)
 * @param  max: a pointer the value(s) to update
 */
void
maximum(unsigned short *val, unsigned short *max);

/**
 * @brief  ordering function, if val is lesser than min then update min
 * @param  val: a pointer to the input value(s)
 * @param  min: a pointer the value(s) to update
 */
void
minimum(unsigned short *val, unsigned short *min);

#endif
