//
// doLCE - do Lenticular film Color rEconstruction -
// Copyright (C) 2012 Joakim Reuteler

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public Licenseversion 3 as 
// published by the Free Software Foundation.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// _______________________________________________________________________ 

/* 2018-02-05 purge code */



// - declarations of simple operations on images
#ifndef doLCE_imageProcessing_h
#define doLCE_imageProcessing_h

#include "imageHandling.h"

// *** unary operations ***
// ************************
int addGl_glImage( glImage_t *glImage, int gl );

int multiplyGl_glImage( glImage_t *glImage, float factor );

int setValue_rgbImage( rgbImage_t *rgbImage, int value );

int multiplyVal_rgbImage( rgbImage_t *rgbImage, float factors[3] );

int subtractVal_rgbImage( rgbImage_t *rgbImage, int values[3] );

// *** binary operations ***
// *************************
int add_glImage_to_rgbImage( glImage_t *glImage, rgbImage_t *rgbImage, int channel );
#endif
