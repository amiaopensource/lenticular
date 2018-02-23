/******************************************************************************
 *
 * interpolationStudy.c
 * - RGB image pixel column interpolation study
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



#include "imageHandling.h"
#include "imageProcessing.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>



int main( int argc, char *argv[] ) {
  int status = 0;

  // constants
  char greetingText[128] = "";
  sprintf( greetingText, "\nmodified inStudy 2018-02-24 alpha\n  RGB image pixel column interpolation study\n\n" );
  char helpText[] = "inStudy [--help] 'inputFileName'\n\n";
  char inputImageName[128] = "";
  char outputImageName[128] = "";

  // variables
  int argNo, optNo;
  short spp, bps;
  int c, i, j;
  int width, height;
  float a, b;

  // data
  rgbImage_t inImg;
  rgbImage_t outImg;

  printf( "%s", greetingText );
  if ( argc < 2 ) {
    printf( "%s", helpText );
    exit(0);
  } else {
    optNo = 0;
    argNo = 1;
    while ( argNo < argc ) {
      if ( argv[argNo][0] == '-' ) {
        optNo++;
        printf( "WARNING: Option '%s' ignored at the moment.\n", argv[argNo] );
      }
      argNo++;
    }
    if ( optNo + 1 != argc - 1 )
      printf( "WARNING: Several input images specified, only first one will be read at the\n  moment.\n" );
    strcat( inputImageName, argv[optNo + 1] );
    printf( "image to load: %s\n", inputImageName );
    sprintf( outputImageName, "study_%s", inputImageName );
    printf( "result will be written to '%s'\n", outputImageName );
    printf( "Checking input image...\n" );
    status = check_TIFF( inputImageName, &spp, &bps, &width, &height );
    if ( status == 0 ) {
      printf( "> % 5d samples per pixel\n> % 5d bits per sample\n", spp, bps );
      printf( "> % 5d pixels wide\n> % 5d pixels high\n", width, height );
      if ( spp != 3 || bps != 16 ) {
        printf( "ERROR: Input image must be 16-bit RGB.\n" );
        exit(1);
      } else {
        status = new_rgbImage( &inImg, width, height );
        if ( status == 0 ) {
          printf( "### allocated memory: 'inImg.memState = %d'\n", inImg.memState );
          printf( "Loading image...\n" );
          status = read_3x16bitTIFF_rgbImage( inputImageName, &inImg );
          if ( status == 0 )
            printf( "> OK\n" );
        }
      }
    }
    printf( "\n" );
  }
  if ( status == 0 ) {
    status = new_rgbImage( &outImg, inImg.width, inImg.height );
    if ( status == 0 ) {
      printf( "### allocated memory: 'outImg.memState = %d'\n", outImg.memState );
      printf( "Copying pixels with x mod 2 == 1 to 'outImg'...\n" );
      for ( c=0; c<3; c++ )
        for ( j=0; j < inImg.height; j++ )
          for ( i=0; i < inImg.width; i+=2 ) {
            outImg.img[c][j][i] = inImg.img[c][j][i];
            outImg.img[c][j][i+1] = 0;
          }
      printf( "> OK\n" );
    }
  }
  if ( status == 0 ) {
    printf( "Now interpolating the lost pixel values...\n" );
    a = 3.0;
    b = 2.0;
    for ( c=0; c<3; c++ )
      for ( j=1; j < outImg.height-1; j++ )
        for ( i=1; i < outImg.width-1; i+=2 )
          outImg.img[c][j][i] = (int)((a*(outImg.img[c][j][i-1]+outImg.img[c][j][i+1]) + b*(outImg.img[c][j-1][i-1]+outImg.img[c][j-1][i+1]+outImg.img[c][j+1][i-1]+outImg.img[c][j+1][i+1]))/(2*a+4*b));
  }
  if ( status == 0 ) {
    printf( "Writing result to disk...\n" );
    status = write_3x16bitTIFF_rgbImage( &outImg, outputImageName );
    if ( status == 0 )
      printf( "Stored result to '%s'.\n", outputImageName );
  }
  if ( inImg.memState != 0 ) {
    printf( "Cleaning up heap...\n" );
    delete_rgbImage( &inImg );
    printf( "### freed memory: 'inImg.memState = %d'\n", inImg.memState );
  }
  if ( outImg.memState != 0 ) {
    delete_rgbImage( &outImg );
    printf( "### freed memory: 'outImg.memState = %d'\n", inImg.memState );
  }
  return status;
}
