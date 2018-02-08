//
// doLCE - do Lenticular film Color rEconstruction -
// Copyright (C) 2012 Joakim Reuteler

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// _______________________________________________________________________

/******************************************************************************
 *
 * HISTORY
 *   2018-02-05 purge code
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
  sprintf( greetingText, "*\n**\n*** inStudy - rgb image pixel column interpolation Study ***\n**\n*   [%s ~ %s]\n\n", __DATE__, __TIME__);
  char copyrightText[200] = "This program is free software under the terms of the GNU General Public License version 3.\nSee <http://www.gnu.org/licenses/>.\n\n";
  char helpText[] = "inStudy [--help] 'inputFileName'\n";
  char inputImageName[128] = "";
  char outputImageName[128] = "";

  // variables
  int argNo, optNo;
  short spp, bps;
  int width, height;
  int x, y;
  int j, i;
  int channel;
  float a, b;

  // data
  rgbImage_t inImg;
  rgbImage_t outImg;

  printf( "%s", greetingText );
  printf( "%s", copyrightText );

  if ( argc < 2 ) {
    status = -1;
    printf( "Too few arguments.\n" );
    printf( "\n" );
    printf( "%s", helpText );
  } else {
    optNo = 0;
    argNo = 1;
    while ( argNo < argc ) {
      if ( argv[argNo][0] == '-' ) {
        optNo++;
        printf( "option '%s' ignored at the moment\n", argv[argNo] );
      }
      argNo++;
    }
    if ( optNo+1 != argc-1 ) {
      printf( "WARNING: Several input images specified, only first one will be read at the moment.\n" );
    }
    strcat( inputImageName, argv[optNo+1] );
    printf( "image to load : %s\n", inputImageName );
    sprintf( outputImageName, "study_%s", inputImageName );
    printf( "result will be written to '%s'\n", outputImageName );
  }
  if ( status == 0 ) {
    printf( "Checking input image...\n" );
    status = check_TIFF( inputImageName, &spp, &bps, &width, &height );
    if ( status == 0 ) {
      printf( "> % 5d samples per pixel \n> % 5d bits per sample\n", spp, bps );
      printf( "> % 5d pixels wide\n> % 5d pixels high\n", width, height );
      if ( spp != 3 || bps != 16 ) {
        status = -1;
        printf( "ERROR: Input image must be 16bit rgb!\n" );
      } else {
        status = new_rgbImage( &inImg, width, height );
        if ( status == 0 ) {
          printf( "### allocated memory : 'inImg.memState = %d'\n", inImg.memState );
          printf( "Loading image...\n" );
          status = read_3x16bitTIFF_rgbImage( inputImageName, &inImg );
          if ( status == 0 ) {
            printf( "> OK.\n" );
          }
        }
      }
    }
    printf( "\n" );
  }
  if ( status == 0 ) {
    status = new_rgbImage( &outImg, inImg.width, inImg.height );
    if ( status == 0 ) {
      printf( "### allocated memory : 'outImg.memState = %d'\n", outImg.memState );
      printf( "Copying pixels with x mod 2 == 1 to 'outImg'...\n" );
      for ( channel=0; channel<3; channel++ ) {
        for ( j=0; j<inImg.height; j++ ) {
          for ( i=0; i<inImg.width; i+=2 ) {
            outImg.img[channel][j][i] = inImg.img[channel][j][i];
            outImg.img[channel][j][i+1] = 0;
          }
        }
      }
      printf( "> OK.\n" );
    }
  }
  if ( status == 0 ) {
    printf( "Now interpolating the lost pixel values...\n" );
    a = 3.0;
    b = 2.0;
    for ( channel=0; channel<3; channel++ ) {
      for ( j=1; j<outImg.height-1; j++ ) {
        for ( i=1; i<outImg.width-1; i+=2 ) {
          outImg.img[channel][j][i] = (int)((a*(outImg.img[channel][j][i-1]+outImg.img[channel][j][i+1]) + b*(outImg.img[channel][j-1][i-1]+outImg.img[channel][j-1][i+1]+outImg.img[channel][j+1][i-1]+outImg.img[channel][j+1][i+1]))/(2*a+4*b));
        }
      }
    }
  }
  if ( status == 0 ) {
    printf( "Writing result to disk...\n" );
    status = write_3x16bitTIFF_rgbImage( &outImg, outputImageName );
    if ( status == 0 ) {
      printf( "Stored result to '%s'.\n", outputImageName );
    }
  }
  if ( inImg.memState != 0 ) {
    printf( "Cleaning up heap...\n" );
    delete_rgbImage( &inImg );
    printf( "### freed memory 'inImg.memState = %d'\n", inImg.memState );
  }
  if ( outImg.memState != 0 ) {
    delete_rgbImage( &outImg );
    printf( "### freed memory 'outImg.memState = %d'\n", inImg.memState );
  }

  printf( "\nStatus at end : %d\n", status );
  return status;
}
