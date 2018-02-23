/******************************************************************************
 *
 * imageProcessing.h
 * - declare simple operations on images
 *
 * This file is part of 'lenticular'.
 *
 * Copyright (c) 2012 Joakim Reuteler
 * Copyright (c) 2018 AMIA Open Source
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 ******************************************************************************/



#ifndef imageProcessing
#define imageProcessing

#include "imageHandling.h"

// unary operations

int addGl_glImage( glImage_t *glImage, int gl );
int multiplyGl_glImage( glImage_t *glImage, float factor );
int setValue_rgbImage( rgbImage_t *rgbImage, int value );
int multiplyVal_rgbImage( rgbImage_t *rgbImage, float factors[3] );
int subtractVal_rgbImage( rgbImage_t *rgbImage, int values[3] );

// binary operation

int add_glImage_to_rgbImage( glImage_t *glImage, rgbImage_t *rgbImage, int channel );

#endif
