/******************************************************************************
 *
 * imageHandling.c
 * - memory handling for grey-level and RGB images
 * - read and write TIFF images
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

#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>



// grey-level and RGB image types

int new_glImage( glImage_t *glImage, int width, int height ) {
  int status = 0;
  int j;

  if ( glImage->img != NULL || glImage->memState != 0 ) {
    status = -1;
    printf( "ERROR: new_glImage(): Invalid argument 'glImage->img != NULL || glImage->memState != 0'.\n" );
  } else if ( width < 1 || height < 1 ) {
    status = -1;
    printf( "ERROR: new_glImage(): Invalid argumment 'width < 1 || heigth < 1'.\n" );
  }
  if ( status == 0 ) {
    glImage->width = width;
    glImage->height = height;
    glImage->img = malloc( (long)(glImage->height) * sizeof(int*) );
    if ( glImage->img == NULL ) {
      status = -1;
      printf( "ERROR: new_glImage(): Failed to allocate memory for 'glImage->img'.\n" );
    }
    for ( j=0; j < glImage->height && status == 0; j++ ) {
      glImage->img[j] = malloc( (long)(glImage->width) * sizeof(int) );
      if ( glImage->img[j] == NULL ) {
        status = -1;
        printf( "ERROR: new_glImage(): Failed to allocate memory for 'glImage->img[%d]'.\n", j );
      }
    }
  }
  if ( status == 0 )
    glImage->memState = 1;
  else
    glImage->memState = -1;

  return status;
}



int delete_glImage( glImage_t *glImage ) {
  int status = 0;
  int j;

  if ( glImage->img == NULL || glImage->memState == 0 ) {
    status = -1;
    printf( "ERROR: delete_glImage(): Nothing to do 'glImage->img == NULL || glImage->memState == 0'.\n" );
  } else if ( glImage->width < 1 || glImage->height < 1 ) {
    status = -1;
    printf( "ERROR: delete_glImage(): Invalid argument 'glImage->width < 1 || glImage->height < 1'.\n" );
  } else {
    for ( j=0; j < glImage->height; j++ )
      free( glImage->img[j] );
    free( glImage->img );
    glImage->img = NULL;
    glImage->memState = 0;
  }

  return status;
}



// new RGB image

int new_rgbImage( rgbImage_t *rgbImage, int width, int height ) {
  int status = 0;
  int c, j;

  if ( rgbImage->img != NULL || rgbImage->memState != 0 ) {
    status = -1;
    printf( "ERROR: new_bitstore_rgbImage(): Invalid argument 'rgbImage->img != NULL || rgbImage->memState != 0'.\n" );
  } else if ( width < 1 || height < 1 ) {
    status = -1;
    printf( "ERROR: Won't allocate memory because 'width < 1 || heigth < 1'.\n" );
  } else {
    rgbImage->width = width;
    rgbImage->height = height;
    rgbImage->img = malloc( (long)(3) * sizeof(int**) );
    if ( rgbImage->img == NULL ) {
      status = -1;
      printf( "ERROR: new_bitstore_rgbImage(): Failed to allocate memory for 'rgbImage->img'.\n" );
    }
    for ( c=0; c<3 && status == 0; c++ ) {
      rgbImage->img[c] = malloc( (long)(rgbImage->height) * sizeof(int*) );
      if ( rgbImage->img[c] == NULL )
        printf( "ERROR: new_bitstore_rgbImage(): Failed to allocate memory for 'rgbImage->img[%d]'.\n", c );
      for ( j=0; j < rgbImage->height && status == 0; j++ ) {
        rgbImage->img[c][j] = malloc( (long)(rgbImage->width) * sizeof(int) );
        if ( rgbImage->img[c][j] == NULL ) {
          status = -1;
          printf( "ERROR: new_bitstore_rgbImage(): Failed to allocate memory for 'rgbImage->img[%d][%d]'.\n", c, j );
        }
      }
    }
  }
  if ( status == 0 )
    rgbImage->memState = 1;
  else
    rgbImage->memState = -1;

  return status;
}



// delete RGB image

int delete_rgbImage( rgbImage_t *rgbImage ) {
  int status = 0;
  int c, j;

  if ( rgbImage->img == NULL && rgbImage->memState == 0 ) {
    status = -1;
    printf( "ERROR: delete_rgbImage(): Nothing to do 'rgbImage->img == NULL && rgbImage->memState == 0'.\n" );
  } else if ( rgbImage->width < 1 || rgbImage->height < 1 ) {
    status = -1;
    printf( "ERROR: delete_rgbImage(): Invalid argument 'rgbImage->width < 1 || rgbImage->height < 1'.\n" );
  } else if ( rgbImage->memState != 1 ) {
    status = -1;
    printf( "ERROR: delete_rgbImage(): Invalid argument 'rgbImage->memState != 1'.\n" );
  } else {
    for ( c=0; c<3; c++ ) {
      for ( j=0; j < rgbImage->height; j++ )
        free( rgbImage->img[c][j] );
      free( rgbImage->img[c] );
    }
    free( rgbImage->img );
    rgbImage->img = NULL;
    rgbImage->memState = 0;
  }

  return status;
}



// TIFF handling

// check TIFF

int check_TIFF( char *fileName, short *spp, short *bps, int *width, int *height ) {
  int status = 0;
  *spp = *bps = -1;
  TIFF *inTIFF;
  inTIFF = NULL;

  if( (inTIFF = TIFFOpen( fileName, "r" )) == NULL ) {
    status = -1;
    printf( "ERROR: check_TIFF(): Failed to open '%s'.\n", fileName );
  } else {
    TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, spp );
    TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, bps );
    TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, width );
    TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, height );
  }
  if ( *spp == -1 || *bps == -1 ) {
    status = -1;
    printf( "ERROR: Failed to read TIFF-tags in '%s'.\n", fileName );
  }
  if( inTIFF != NULL )
    TIFFClose( inTIFF );

  return status;
}



int read_glTIFF( char *inputTIFFname, glImage_t *glImage ) {
  int status = 0;
  int i, j;
  unsigned char *lineBuffer;
  int width, height;
  short spp, bps;
  TIFF *inTIFF;
  inTIFF = NULL;

  if ( glImage->img == NULL || glImage->memState != 1 ) {
    status = -1;
    printf( "ERROR: Invalid argument 'glImage->img == NULL || glImage->memState != 1'.\n" );
  }
  if ( status == 0 )
    if ( (inTIFF = TIFFOpen( inputTIFFname, "r" )) == NULL ) {
      status = -1;
      printf( "ERROR: Failed to open TIFF.\n" );
    }
  if ( status == 0 ) {
    TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, &spp );
    TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
    TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, &width );
    TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, &height );
    if( spp!=1 || bps!=8 || glImage->width != width || glImage->height != height ) {
      status = -1;
      printf( "ERROR: Can't read '%s' because input parameters are inconsistent:\n", inputTIFFname );
      printf( "  actual parameters:   spp=%d, bps=%d, width=%d, height%d\n", spp, bps, width, height );
      printf( "  expected parameters: spp=%d, bps=%d, width=%d, heigth%d\n", 1, 8, glImage->width, glImage->height );
    }
  }
  if ( status == 0 ) {
    lineBuffer = malloc( (long)(glImage->width)*sizeof(unsigned char) );
    if ( lineBuffer == NULL ) {
      status = -1;
      printf( "ERROR: Failed to allocate memory for lineBuffer.\n" );
    } else {
      for ( j=0; j < glImage->height; j++ ) {
        if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ) {
          printf( "ERROR: Failed to read scanline %d from %s.\n", j, inputTIFFname );
          status = -1;
        } else {
          for( i=0; i < glImage->width; i++ )
            glImage->img[j][i] = (int)lineBuffer[i];
        }
      }
      free( lineBuffer );
    }
  }
  if ( inTIFF != NULL )
    TIFFClose( inTIFF );

  return status;
}



int read_16bitTIFF_glImage( char *TIFFname, glImage_t *glImage ) {
  int status = 0;
  int i, j;
  uint16 *lineBuffer; // here changes will be done and accordingly below
  int width, height;
  short spp, bps;
  TIFF *inTIFF;
  inTIFF = NULL;

  if ( glImage->img == NULL || glImage->memState != 1 ) {
    status = -1;
    printf( "ERROR: read_16bitTIFF_glImage(): Invalid argument 'glImage->img == NULL || glImage->memState != 1'.\n" );
  } else {
    if ( (inTIFF = TIFFOpen( TIFFname, "r" )) == NULL ) {
      status = -1;
      printf( "ERROR: Failed to open TIFF.\n" );
    }
  }
  if ( status == 0 ) {
    TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, &spp );
    TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
    TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, &width );
    TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, &height );
    if( spp!=1 || bps!=16 || glImage->width != width || glImage->height != height ) {
      status = -1;
      printf( "ERROR: read_16bitTIFF_glImage(): Can't read '%s' because input parameters are inconsistent:\n", TIFFname );
      printf( "  actual parameters:   spp=%d, bps=%d, width=%d, height%d\n", spp, bps, width, height );
      printf( "  expected parameters: spp=%d, bps=%d, width=%d, heigth%d\n", 1, 16, glImage->width, glImage->height );
    }
  }
  if ( status == 0 ) {
    lineBuffer = malloc( (long)(glImage->width) * sizeof(uint16) );
    if ( lineBuffer == NULL ) {
      status = -1;
      printf( "ERROR: read_16bitTIFF_glImage(): Failed to allocate memory for 'lineBuffer'.\n" );
    } else {
      for ( j=0; j < glImage->height; j++ ) {
        if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ) {
          printf( "ERROR: read_16bitTIFF_glImage(): Failed to read scanline %d from %s.\n", j, TIFFname );
          status = -1;
        } else {
          for( i=0; i < glImage->width; i++ )
            glImage->img[j][i] = (int)lineBuffer[i];
        }
      }
      free( lineBuffer );
    }
  }
  if ( inTIFF != NULL )
    TIFFClose( inTIFF );

  return status;
}



int read_3x8bitTIFF_rgbImage( char *fromTIFFname, rgbImage_t *rgbImage ) {
  int status = 0;
  int i, j;
  uint8 *lineBuffer;
  int width, height;
  short spp, bps;
  TIFF *inTIFF;
  inTIFF = NULL;

  if ( rgbImage->img == NULL || rgbImage->memState != 1 ) {
    status = -1;
    printf( "ERROR: read_3x8bitTIFF_grgbImage(): Invalid argument 'rgbImage->img == NULL || rgbImage->memState != 1'.\n" );
  } else {
    if ( (inTIFF = TIFFOpen( fromTIFFname, "r" )) == NULL ) {
      status = -1;
      printf( "ERROR: Failed to open TIFF.\n" );
    }
  }
  if ( status == 0 ) {
    TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, &spp );
    TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
    TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, &width );
    TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, &height );
    if( spp!=3 || bps!=8 || rgbImage->width != width || rgbImage->height != height ) {
      status = -1;
      printf( "ERROR: read_3x8bitTIFF_rgbImage(): Can't read '%s' because input parameters are inconsistent:\n", fromTIFFname );
      printf( "  actual parameters:   spp=%d, bps=%d, width=%d, height%d\n", spp, bps, width, height );
      printf( "  expected parameters: spp=%d, bps=%d, width=%d, heigth%d\n", 3, 8, rgbImage->width, rgbImage->height );
    }
  }
  if ( status == 0 ) {
    lineBuffer = malloc( (long)(rgbImage->width) * 3 * sizeof(uint8) );
    if ( lineBuffer == NULL ) {
      status = -1;
      printf( "ERROR: read_3x8bitTIFF_rgbImage(): Failed to allocate memory for 'lineBuffer'.\n" );
    } else {
      for ( j=0; j < rgbImage->height; j++ )
        if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ) {
          printf( "ERROR: read_3x8bitTIFF_rgbImage(): Failed to read scanline %d from %s.\n", j, fromTIFFname );
          status = -1;
        } else {
          for( i=0; i < rgbImage->width; i++ ) {
            rgbImage->img[0][j][i] = (int)lineBuffer[3 * i];
            rgbImage->img[1][j][i] = (int)lineBuffer[3 * i + 1];
            rgbImage->img[2][j][i] = (int)lineBuffer[3 * i + 2];
          }
        }
      free( lineBuffer );
    }
  }
  if ( inTIFF != NULL )
    TIFFClose( inTIFF );

  return status;
}



int read_3x16bitTIFF_rgbImage( char *fromTIFFname, rgbImage_t *rgbImage ) {
  int status = 0;
  int i, j;
  uint16 *lineBuffer; // here changes will be done and accordingly below
  int width, height;
  short spp, bps;
  TIFF *inTIFF;
  inTIFF = NULL;

  if ( rgbImage->img == NULL || rgbImage->memState != 1 ) {
    status = -1;
    printf( "ERROR: read_3x16bitTIFF_grgbImage(): Invalid argument 'rgbImage->img == NULL || rgbImage->memState != 1'.\n" );
  }
  if ( status == 0 )
    if ( (inTIFF = TIFFOpen( fromTIFFname, "r" )) == NULL ) {
      status = -1;
      printf( "ERROR: Failed to open TIFF.\n" );
    }
  if ( status == 0 ) {
    TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, &spp );
    TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
    TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, &width );
    TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, &height );
    if( spp!=3 || bps!=16 || rgbImage->width != width || rgbImage->height != height ) {
      status = -1;
      printf( "ERROR: read_3x16bitTIFF_rgbImage(): Can't read '%s' because input parameters are inconsistent:\n", fromTIFFname );
      printf( "  actual parameters:   spp=%d, bps=%d, width=%d, height%d\n", spp, bps, width, height );
      printf( "  expected parameters: spp=%d, bps=%d, width=%d, heigth%d\n", 3, 16, rgbImage->width, rgbImage->height );
    }
  }
  if ( status == 0 ) {
    lineBuffer = malloc( (long)(rgbImage->width) * 3 * sizeof(uint16) );
    if ( lineBuffer == NULL ) {
      status = -1;
      printf( "ERROR: read_3x16bitTIFF_rgbImage(): Failed to allocate memory for 'lineBuffer'.\n" );
    } else {
      for ( j=0; j < rgbImage->height; j++ ) {
        if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ) {
          printf( "ERROR: read_3x16bitTIFF_rgbImage(): Failed to read scanline %d from %s.\n", j, fromTIFFname );
          status = -1;
        } else {
          for( i=0; i < rgbImage->width; i++ ) {
            rgbImage->img[0][j][i] = (int)lineBuffer[3 * i];
            rgbImage->img[1][j][i] = (int)lineBuffer[3 * i + 1];
            rgbImage->img[2][j][i] = (int)lineBuffer[3 * i + 2];
          }
        }
      }
      free( lineBuffer );
    }
  }
  if ( inTIFF != NULL )
    TIFFClose( inTIFF );

  return status;
}



int write_16bitTIFF_glImage( glImage_t *glImage, char *toTIFFname ) {
  int status = 0;
  int i, j;
  int minGl, maxGl;
  TIFF *toTIFF;
  uint16 *lineBuffer;

  if ( glImage->img == NULL || glImage->memState != 1 ) {
    status = -1;
    printf( "ERROR: write_16bitTIFF_glImage(): Invalid argument 'glImage->img == NULL || glImage->memState != 1'.\n" );
  } else {
    status = get_range_glImage( glImage, &minGl, &maxGl );
    if ( status != 0 ) {
      printf( "ERROR: write_16bitTIFF_glImage(): Cannot determine grey-level range.\n" );
    } else if ( minGl < 0 || maxGl > 65535 ) {
      status = -1;
      printf( "ERROR: write_16bitTIFF_glImage(): grey-level out of range: 'minGl = %d || maxGl = %d'.\n", minGl, maxGl );
    }
  }
  if ( status == 0 ) {
    if ( (toTIFF = TIFFOpen(toTIFFname, "w")) == NULL ) {
      status = -1;
      printf( "ERROR: write_16bitTIFF_glImage(): Could not open '%s' for writing.\n", toTIFFname );
    }
    TIFFSetField( toTIFF, TIFFTAG_IMAGEWIDTH, glImage->width );
    TIFFSetField( toTIFF, TIFFTAG_IMAGELENGTH, glImage->height );
    TIFFSetField( toTIFF, TIFFTAG_BITSPERSAMPLE, 16 );
    TIFFSetField( toTIFF, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField( toTIFF, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField( toTIFF, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    lineBuffer = malloc( (long)(glImage->width)*sizeof(uint16) );
    if ( lineBuffer == NULL ) {
      status = -1;
      printf( "ERROR: write_16bitTIFF_glImage(): Failed to allocate memory for 'lineBuffer'.\n" );
    } else {
      for ( j=0; j < glImage->height; j++ ) {
        for ( i=0; i < glImage->width && status == 0; i++ )
          lineBuffer[i] = (uint16) glImage->img[j][i];
        if ( TIFFWriteScanline( toTIFF, lineBuffer, j, 0 ) == -1 ) {
          status = -1;
          printf ( "ERROR: write_16bitTIFF_glImage(): Failed to write scan line (%d)! status = %d.\n", j, status );
        }
      }
      free( lineBuffer );
    }
    TIFFClose( toTIFF );
  }

  return status;
}



int write_3x8bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFname ) {
  int status = 0;
  int c, i, j;
  int min[3], max[3];
  int width, height;
  TIFF *outTIFF;
  unsigned char *lineBuffer;

  if ( rgbImage->img == NULL || rgbImage->memState != 1 ) {
    status = -1;
    printf ( "ERROR: write_3x8bitTIFF_rgbImage(): Inavlid argument 'rgbImage->img == NULL || rgbImage->memState != 1'.\n" );
  }
  if ( status == 0 )
    status = get_range_rgbImage( rgbImage, min, max );
  for ( c=0; c<2 && status == 0; c++ )
    if ( min[c]<0 || max[c]>255 ) {
      status = -1;
      printf ( "ERROR: write_3x8bitTIFF_rgbImage(): Values out of range for 8-bit RGB: red %d to %d, green %d to %d, blue %d to %d.\n", min[0], max[0], min[1], max[1], min[2], max[2] );
    }
  if ( (outTIFF = TIFFOpen(toTIFFname, "w")) == NULL ) {
    status = -1;
    printf( "ERROR: write_3x8bitTIFF_rgbImage(): Failed to open '%s' for writing.\n", toTIFFname );
  }
  if ( status == 0 ) {
    width = rgbImage->width;
    height = rgbImage->height;
    TIFFSetField( outTIFF, TIFFTAG_IMAGEWIDTH, width );
    TIFFSetField( outTIFF, TIFFTAG_IMAGELENGTH, height );
    TIFFSetField( outTIFF, TIFFTAG_BITSPERSAMPLE, 8 );
    TIFFSetField( outTIFF, TIFFTAG_SAMPLESPERPIXEL, 3 );
    TIFFSetField( outTIFF, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField( outTIFF, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    if ( (lineBuffer = malloc( 3*width*sizeof(unsigned char) )) == NULL ) {
      status = -1;
      printf ( "ERROR: write_3x8bitTIFF_rgbImage(): Failed to allocate memory for 'lineBuffer'.\n" );
    }
    for ( j=0; j < height && status == 0; j++ ) {
      for ( i=0; i < width; i++ )
        for ( c=0; c<3; c++ )
          lineBuffer[3 * i + c] = (unsigned char)(rgbImage->img[c][j][i]);
      if (TIFFWriteScanline (outTIFF, lineBuffer, j, 0) == -1) {
        status = -1;
        printf ( "ERROR: write_3x8bitTIFF_rgbImage(): Failed to write scan line (%d).\n", j );
      }
    }
    free( lineBuffer );
    TIFFClose( outTIFF );
  }

  return status;
}



int writeROI_3x8bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFname, int roiXpos, int roiYpos, int roiWidth, int roiHeight ) {
  int status = 0;
  int c, i, j;
  int min[3], max[3];
  int width, height;
  TIFF *outTIFF;
  unsigned char *lineBuffer;

  if ( rgbImage->img == NULL || rgbImage->memState != 1 ) {
    status = -1;
    printf ( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Invalid argument 'rgbImage->img == NULL || rgbImage->memState != 1'.\n" );
  }
  if ( roiXpos + roiWidth > rgbImage->width || roiYpos + roiHeight > rgbImage->height ) {
    status = -1;
    printf ( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Invalid arguments 'roiXpos+roiWidth > rgbImage.width || roiYpos+roiheight > rgbImage.height'.\n" );
  }
  if ( status == 0 )
    status = get_range_rgbImage( rgbImage, min, max );
  for ( c=0; c<2 && status == 0; c++ ) {
    if ( min[c] < 0 || max[c] > 255 ) {
      status = -1;
      printf ( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Values out of range for 8-bit RGB: red %d to %d, green %d to %d, blue %d to %d.\n", min[0], max[0], min[1], max[1], min[2], max[2] );
    }
  }
  if ( (outTIFF = TIFFOpen(toTIFFname, "w")) == NULL ) {
    status = -1;
    printf( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Failed to open '%s' for writing.\n", toTIFFname );
  }
  if ( status == 0 ) {
    width = roiWidth;
    height = roiHeight;
    TIFFSetField( outTIFF, TIFFTAG_IMAGEWIDTH, width );
    TIFFSetField( outTIFF, TIFFTAG_IMAGELENGTH, height );
    TIFFSetField( outTIFF, TIFFTAG_BITSPERSAMPLE, 8 );
    TIFFSetField( outTIFF, TIFFTAG_SAMPLESPERPIXEL, 3 );
    TIFFSetField( outTIFF, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField( outTIFF, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    if ( (lineBuffer = malloc( 3*width*sizeof(unsigned char) )) == NULL ) {
      status = -1;
      printf ( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Failed to allocate memory for line buffer.\n" );
    }
    for ( j=0; j < roiHeight && status == 0; j++ ) {
      for ( i=0; i < roiWidth; i++ )
        for ( c=0; c<3; c++ )
          lineBuffer[3*i+c] = (unsigned char)(rgbImage->img[c][j + roiYpos][i + roiXpos]);
      if (TIFFWriteScanline (outTIFF, lineBuffer, j, 0) == -1) {
        status = -1;
        printf ( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Failed to write scan line (%d).\n", j );
      }
    }
    free( lineBuffer );
    TIFFClose( outTIFF );
  }

  return status;
}



int write_3x16bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFname ) {
  int status = 0;
  int c, i, j;
  int min[3], max[3];
  int width, height;
  TIFF *outTIFF;
  uint16 *lineBuffer;

  if ( rgbImage->img == NULL || rgbImage->memState != 1 ) {
    status = -1;
    printf ( "ERROR: write_3x16bitTIFF_rgbImage(): Invalid argument 'rgbImage->img == NULL || rgbImage->memState != 1'.\n" );
  }
  if ( status == 0 )
    status = get_range_rgbImage( rgbImage, min, max );
  for ( c=0; c<2 && status == 0; c++ )
    if ( min[c] < 0 || max[c] > 65535 ) {
      status = -1;
      printf ( "ERROR: write_3x16bitTIFF_rgbImage(): Values out of range for 16-bit RGB: red %d to %d, green %d to %d, blue %d to %d.\n", min[0], max[0], min[1], max[1], min[2], max[2] );
    }
  if ( (outTIFF = TIFFOpen(toTIFFname, "w")) == NULL ) {
    status = -1;
    printf( "ERROR: write_3x16bitTIFF_rgbImage(): Failed to open '%s' for writing.\n", toTIFFname );
  }
  if ( status == 0 ) {
    width = rgbImage->width;
    height = rgbImage->height;
    TIFFSetField( outTIFF, TIFFTAG_IMAGEWIDTH, width );
    TIFFSetField( outTIFF, TIFFTAG_IMAGELENGTH, height );
    TIFFSetField( outTIFF, TIFFTAG_BITSPERSAMPLE, 16 );
    TIFFSetField( outTIFF, TIFFTAG_SAMPLESPERPIXEL, 3 );
    TIFFSetField( outTIFF, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField( outTIFF, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    if ( (lineBuffer = malloc( (long)(rgbImage->width) * 3 * sizeof(uint16) )) == NULL ) {
      status = -1;
      printf ( "ERROR: write_3x16bitTIFF_rgbImage(): Failed to allocate memory for 'lineBuffer'.\n" );
    }
    for ( j=0; j < height && status == 0; j++ ) {
      for ( i=0; i < width; i++ )
        for ( c=0; c<3; c++ )
          lineBuffer[3*i+c] = (uint16)(rgbImage->img[c][j][i]);
      if (TIFFWriteScanline (outTIFF, lineBuffer, j, 0) == -1) {
        status = -1;
        printf ( "ERROR: write_3x8bitTIFF_rgbImage(): Failed to write scan line (%d).\n", j );
      }
    }
    free( lineBuffer );
    TIFFClose( outTIFF );
  }

  return status;
}



// basic information on image content

int get_range_glImage( glImage_t *glImage, int *minGl, int *maxGl ) {
  int status = 0;
  int i, j;

  if ( glImage->img == NULL ) {
    status = -1;
    printf( "ERROR: get_range_glImage(): Can't determine mix/max for an empty grey-level image.\n" );
  }
  *minGl = glImage->img[0][0];
  *maxGl = *minGl;
  for ( j=0; j < glImage->height; j++ ) {
    for ( i=0; i < glImage->width; i++ ) {
      if ( glImage->img[j][i] > *maxGl )
        *maxGl = glImage->img[j][i];
      if ( glImage->img[j][i] < *minGl )
        *minGl = glImage->img[j][i];
    }
  }

  return status;
}



int get_range_rgbImage( rgbImage_t *rgbImage, int minValue[3], int maxValue[3] ) {
  int status = 0;
  int c, i, j;
  int min, max;
  int width, height;

  if ( rgbImage->img == NULL || rgbImage->memState != 1 ) {
    status = -1;
    printf ( "ERROR: get_range_rgbImage(): Inavlid argument 'rgbImage->img == NULL || rgbImage->memState != 1'.\n" );
  } else {
    width = rgbImage->width;
    height = rgbImage->height;
    for ( c=0; c<3; c++ ) {
      min = max = rgbImage->img[c][0][0];
      for ( j=0; j < height; j++ )
        for ( i=0; i < width; i++ ) {
          if ( rgbImage->img[c][j][i] < min )
            min = rgbImage->img[c][j][i];
          if ( rgbImage->img[c][j][i] > max )
            max = rgbImage->img[c][j][i];
        }
      minValue[c] = min;
      maxValue[c] = max;
    }
  }

  return status;
}



int new_glHistogram( glHistogram_t *glHist, int minGl, int maxGl ) {
  int status = 0;

  if ( glHist->freq != NULL || glHist->memState != 0 ) {
    status = -1;
    printf( "ERROR: new_glHistogram(): Invalid arguments 'glHist->freq != NULL || glHist->memState != 0'.\n" );
  } else if ( maxGl-minGl <= 0 ) {
    status = -1;
    printf( "ERROR: new_glHistogram(): Invalid grey-level dynamic range: 'maxGl-minGl = %d - %d <= 0'.\n", maxGl, minGl );
  } else {
    glHist->minGl = minGl;
    glHist->maxGl = maxGl;
    glHist->freq = malloc ( (maxGl + 1) * sizeof(unsigned long) );
    if ( glHist->freq == NULL ) {
      status = -1;
      printf( "ERROR: new_glHistogram(): Failed to allocate memory for 'glHist->freq'.\n" );
    }
  }
  if ( status == 0 )
    glHist->memState = 1;
  else
    glHist->memState = -1;

  return status;
}



int delete_glHistogram( glHistogram_t *glHist ) {
  int status = 0;

  if ( glHist->freq == NULL || glHist->memState == 0 ) {
    status = -1;
    printf( "ERROR: delete_glHistogram(): Nothing to do 'glHist->freq == NULL || glHist->memState == 0'.\n" );
  } else {
    free( glHist->freq );
    glHist->freq = NULL;
    glHist->memState = 0;
  }

  return status;
}



int get_glHistogram( glImage_t *glImage, glHistogram_t *hist ) {
  int status = 0;
  int i, j;

  if ( glImage->img == NULL ) {
    status = -1;
    printf( "ERROR: get_glHistogram(): Invalid grey-level image: 'glImage->img == NULL'.\n " );
  } else if ( hist->freq == NULL ) {
    status = -1;
    printf( "ERROR: get_glHistogram(): Invalid grey-level histogram: 'hist->freq == NULL'.\n " );
  } else {
    for ( i=0; i < hist->maxGl + 1; i++ )
      hist->freq[i] = 0;
    for ( j=0; j < glImage->height; j++ )
      for ( i=0; i < glImage->width; i++ )
        hist->freq[glImage->img[j][i]]++;
  }

  return status;
}



int get_glHistograms_fourShot( glImage_t *glImage, glHistogram_t glHists[4] ) {
  int status = 0;
  int s, i = 0, j;

  if ( glImage->img == NULL ) {
    status = -1;
    printf( "ERROR: get_glHistograms_fourShot(): Invalid grey-level image: 'glImage->img == NULL'.\n " );
  } else {
    for ( s=0; s<4 && status == 0; s++ )
      if ( glHists[s].freq == NULL || glHists[s].memState != 1 ) {
        status = -1;
        printf( "ERROR: get_glHistograms_fourShot(): Invalid grey-level histogram: 'glHists[%d].freq == NULL || glHists[i].memState != 1'.\n", i );
      }
  }
  if ( status == 0 ) {
    for ( s=0; s<4; s++ )
      for ( i=0; i < glHists[s].maxGl+1; i++ )
        glHists[s].freq[i] = 0;
    for ( j=0; j < glImage->height - 1; j+=2 ) {
      for ( i=0; i < glImage->width - 1; i+=2 ) {
        glHists[0].freq[glImage->img[j][i]]++;
        glHists[1].freq[glImage->img[j][i+1]]++;
        glHists[2].freq[glImage->img[j+1][i]]++;
        glHists[3].freq[glImage->img[j+1][i+1]]++;
      }
    }
  }

  return status;
}



int write_glHistogram( glHistogram_t *hist, char *fileName ) {
  int status = 0;
  FILE *histFile;
  int gl;

  if ( hist->freq == NULL ) {
    status = -1;
    printf( "ERROR: write_glHistogram(): Can't write empty histogram to disk.\n" );
  } else if ( (hist->minGl == hist->maxGl) && (hist->freq[hist->maxGl] == 0) ) {
    status = -1;
    printf( "ERROR: write_glHistogram(): Can't write empty histogram to disk.\n" );
  } else {
    if ( (histFile = fopen( fileName, "w" )) == NULL ) {
      status = -1;
      printf( "ERROR: write_glHistogram(): Cannot open file '%s' for writing.\n", fileName );
    }
    if ( status == 0 ) {
      fprintf( histFile, "gl\tfreq\n" );
      for ( gl = hist->minGl; gl < hist->maxGl; gl++ )
        fprintf( histFile, "%d\t%lu\n", gl, hist->freq[gl] );
      fclose( histFile );
    }
  }

  return status;
}



int write_glHistograms_fourShot( glHistogram_t glHists[4], char *toFileName ) {
  int status = 0;
  int s;
  FILE *histsFile;
  int gl;
  int minGl, maxGl;

  for ( s=0; s<4 && status == 0; s++ ) {
    if ( glHists[s].minGl != glHists[0].minGl || glHists[s].maxGl != glHists[0].maxGl ) {
      status = -1;
      printf( "ERROR: write_glHistograms_fourShot(): Unequal ranges of the histograms: 'glHists[%d].minGl != glHists[0].minGl || glHists[%d].maxGl != glHists[0].maxGl'.\n", s, s );
    } else if ( glHists[s].freq == NULL || glHists[s].memState != 1 ) {
      status = -1;
      printf( "ERROR: write_glHistograms_fourShot(): Invalid grey-level histogram: 'glHists[%d].freq == NULL || glHists[%d].memState != 1'.\n", s, s );
    } else if ( (glHists[s].minGl == glHists[s].maxGl) && (glHists[s].freq[glHists[s].maxGl] == 0) ) {
      status = -1;
      printf( "ERROR: write_glHistograms_fourShot(): Can't write empty histogram to disk (shot %d).\n", s );
    }
  }
  if ( status == 0 ) {
    minGl = glHists[0].minGl;
    maxGl = glHists[0].maxGl;
    if ( (histsFile = fopen( toFileName, "w" )) == NULL ) {
      status = -1;
      printf( "ERROR: write_glHistograms_fourShot(): Cannot open file %s for writing.\n", toFileName );
    } else {
      fprintf( histsFile, "gl\tfreq(s=0)\freq(s=1)\tfreq(s=2)\tfreq(s=3) where 's' is the shot:\n" );
      for ( gl = minGl; gl < maxGl; gl++ )
        fprintf( histsFile, "%d\t%lu\t%lu\t%lu\t%lu\n", gl, glHists[0].freq[gl], glHists[1].freq[gl], glHists[2].freq[gl], glHists[3].freq[gl] );
      fclose( histsFile );
    }
  }

  return status;
}



int new_glProfile( glProfile_t *glProfile, int length ) {
  int status = 0;

  if ( glProfile->glSum != NULL || glProfile->memState != 0 ) {
    status = -1;
    printf( "ERROR: new_glProfile(): Grey-level profile is not empty 'glProfile->glSum != NULL || glProfile->memState != 0'.\n" );
  } else if ( length < 0 ) {
    status = -1;
    printf( "ERROR: new_glProfile(): Invalid argument 'length < 0'.\n" );
  } else {
    glProfile->p.x = -1;
    glProfile->p.y = -1;
    glProfile->direction = -1;
    glProfile->length = length;
    glProfile->thickness = -1;
    if ( (glProfile->glSum = malloc( length * sizeof(long) )) == NULL ) {
      status = -1;
      glProfile->memState = -1;
      printf ( "ERROR: new_glProfile(): Failed to allocat ememory for 'glProfile->glSum'.\n" );
    } else {
      glProfile->memState = 1;
    }
  }
 
  return status;
}



int delete_glProfile( glProfile_t *glProfile ) {
  int status = 0;

  if ( glProfile->glSum == NULL || glProfile->memState == 0 ) {
    status = -1;
    printf( "ERROR: delete_glProfile(): Cannot free memory 'glProfile->glSum == NULL || glProfile->memState == 0'.\n" );
  } else {
    free( glProfile->glSum );
    glProfile->glSum = NULL;
    glProfile->memState = 0;
  }

  return status;
}



int get_glProfile( glImage_t *glImage, glProfile_t *glProfile, point_t p, int direction, int thickness ) {
  int status = 0;
  int i, j;

  if ( glImage->img == NULL ) {
    status = -1;
    printf( "ERROR: get_glProfile_ROI(): Image is not allocated 'glImage->img == NULL'!\n" );
  } else if ( glProfile->glSum == NULL || glProfile->memState != 1 ) {
    status = -1;
    printf( "ERROR: get_glProfile_ROI(): Profile is not allocated 'glProfile->glSum == NULL' or in unknown state 'glProfile->memState != 1'.\n" );
  } else if ( p.x < 0 || p.x > glImage->width - 1 || p.y < 0 || p.y > glImage->height - 1 ) {
    status = -1;
    printf( "ERROR: get_glProfile_ROI(): Point p=(%d,%d) outside image.\n", p.x, p.y );
  } else {
    if ( direction == 0 ) {
      if ( (p.x + glProfile->length > glImage->width) || (p.y + thickness > glImage->height) ) {
        status = -1;
        printf( "ERROR: get_glProfile_ROI(): ROI (%d,%d) to (%d,%d) outside image.\n", p.x, p.y, p.x + glProfile->length, p.y + thickness );
      }
    } else if ( direction == 1 ) {
      status = -1;
      printf( "SORRY, NOT IMPLEMETED YET.\n" );
    } else {
      status = -1;
      printf( "ERROR: get_glProfile_ROI(): Invalid value 'direction = %d'.\n", direction );
    }
  }
  if ( status == 0 ) {
    glProfile->p.x = p.x;
    glProfile->p.y = p.y;
    glProfile->thickness = thickness;
    glProfile->direction = direction;
    if ( direction == 0 ) {
      for ( i=0; i < glProfile->length; i++ ) {
        glProfile->glSum[i] = 0;
        for ( j=0; j < thickness; j++ )
          glProfile->glSum[i] += glImage->img[p.y+j][p.x+i];
      }
    }
  }

  return status;
}



int write_glProfile( glProfile_t *glProfile, char *fileName ) {
  int status = 0;
  int i, j;
  int x, y;
  FILE *file;

  if ( glProfile->glSum == NULL || glProfile->memState != 1 ) {
    status = -1;
    printf( "ERROR: write_glProfile(): Grey-level profile is empty 'glProfile->glSum == NULL || glProfile->memState != 1'.\n" );
  } else if ( glProfile->length < 0 ) {
    status = -1;
    printf( "ERROR: write_glProfile(): 'glProfile->length < 0'.\n" );
  } else {
    if ( (file=fopen( fileName, "w" )) == NULL ) {
      status = -1;
      printf( "ERROR: write_glProfile(): Failed to open '%s' for writing.\n", fileName );
    } else {
      fprintf( file, "direction: %d \t\t(0 <-> along x-axis, 1 <-> along y-axis\n", glProfile->direction );
      fprintf( file, "point: (%d,%d) \t(upper left point of used rectangle)\n", glProfile->p.x, glProfile->p.y );
      fprintf( file, "length: %d \t\t(along 'direction')\n", glProfile->length);
      fprintf( file, "thickness: %d \t\t(number of grey levels summed, i.e. dimension perpendicular to 'direction')\n", glProfile->thickness );
      if ( glProfile->direction == 0 ) {
        fprintf( file, "x\tglSum\n" );
        x = glProfile->p.x;
        for ( i=0; i < glProfile->length; i++ )
          fprintf( file, "%d\t%ld\n", x+i, glProfile->glSum[i] );
      }
      if ( glProfile->direction == 1 ) {
        fprintf( file, "y\tglSum\n" );
        y = glProfile->p.y;
        for ( j=0; j < glProfile->length; j++ )
          fprintf( file, "%d\t%ld\n", y+j, glProfile->glSum[j] );
      }
      fclose( file );
    }
  }

  return status;
}



int copy_glProfile( glProfile_t *glProfileORI, glProfile_t *glProfileCPY ) {
  int status = 0;
  int i;

  if ( glProfileORI->glSum == NULL || glProfileORI->memState != 1 ) {
    status = -1;
    printf ( "ERROR: copy_glProfile(): Invalid argument 'glProfileORI->glSum == NULL || glProfileORI->memState != 1'.\n" );
  } else if ( glProfileCPY->glSum == NULL || glProfileCPY->memState != 1 ) {
    status = -1;
    printf ( "ERROR: copy_glProfile(): Invalid argument 'glProfileCPY->glSum == NULL || glProfileCPY->memState != 1'.\n" );
  } else if ( glProfileORI->length < 0 ) {
    status = -1;
    printf ( "ERROR: copy_glProfile(): Invalid argument 'glProfileORI->length < 0'.\n" );
  } else if ( glProfileCPY->length < 0 ) {
    status = -1;
    printf ( "ERROR: copy_glProfile(): Invalid argument 'glProfileCPY->length < 0'.\n" );
  } else if ( glProfileORI->length != glProfileCPY->length ) {
    status = -1;
    printf ( "ERROR: copy_glProfile(): Inconsitent length of profiles 'glProfileORI->length != glProfileCPY->length'.\n" );
  } else {
    glProfileCPY->p.x = glProfileORI->p.x;
    glProfileCPY->p.y = glProfileORI->p.y;
    glProfileCPY->thickness = glProfileORI->thickness;
    for ( i=0; i < glProfileORI->length; i++ )
      glProfileCPY->glSum[i] = glProfileORI->glSum[i];
  }

  return status;
}



int smooth_glProfile( glProfile_t *glProfile ) {
  int status = 0;
  int i, ii;
  long buf;
  int kL = 7;
  int kernel[7] = {1, 2, 4, 5, 4, 2, 1};
  int kC = 3;
  int Z = 0;
  for ( i=0; i < kL; i++ )
    Z += kernel[i];
  glProfile_t profileBuf;
  profileBuf.glSum = NULL;
  profileBuf.memState = 0;

  if ( glProfile->glSum == NULL || glProfile->memState != 1 ) {
    status = -1;
    printf ( "ERROR: smooth_glProfile(): Invalid argument 'glProfile->glSum == NULL || glProfile->memeState != 1'.\n" );
  } else if ( glProfile->length < 5 ) {
    status = -1;
    printf ( "ERROR: smooth_glProfile(): Invalid argument 'glProfile->length < 5'.\n" );
  } else {
    status = new_glProfile( &profileBuf, glProfile->length );
    if ( status == 0 )
      status = copy_glProfile( glProfile, &profileBuf );
    if ( status == 0 ) {
      for ( i=0; i < glProfile->length - kL; i++ ) {
        buf = 0;
        for ( ii = 0; ii < kL; ii++ )
          buf += kernel[ii]*profileBuf.glSum[i + ii];
        glProfile->glSum[i + kC] = (long)( (float)buf / Z + 0.5);
      }
    }
    if ( profileBuf.memState != 0 )
      delete_glProfile( &profileBuf );
  }

  return status;
}



int extract_localMinima_glProfile( glProfile_t *profile ) {
  int status = 0;
  int i;
  int min;
  glProfile_t buf;
  buf.glSum = NULL;
  buf.memState = 0;

  if ( profile->glSum == NULL || profile->memState != 1 ) {
    status = -1;
    printf( "ERROR: get_localMinima_glProfile(): Invalid argument 'glProfile->list==NULL || glprofile->memState!=1'.\n" );
  } else {
    status = new_glProfile( &buf, profile->length );
    // 2. compute the 1D erosion and store it in the buffer, boundaries are set to 0 by default
    if ( status == 0 ) {
      buf.glSum[0] = buf.glSum[profile->length - 1] = 0;
      for ( i=1; i < profile->length - 1; i++ ) {
        min = profile->glSum[i];
        if ( profile->glSum[i-1] < min )
          min = profile->glSum[i-1];
        if ( profile->glSum[i+1] < min )
          min = profile->glSum[i+1];
        buf.glSum[i] = min;
      }
    }
    // 3. compare original and eroded profile -> keep only local minima
    if ( status == 0 )
      for ( i=0; i < profile->length; i++ )
        if ( profile->glSum[i] > buf.glSum[i] )
          profile->glSum[i] = 0;
    // 4. delete the eroded profile
    status = delete_glProfile( &buf );
  }

  return status;
}



int extract_wells_glProfile( glProfile_t *profile ) {
  int status = 0;
  int i, ii;
  glProfile_t profileCpy;
  profileCpy.glSum = NULL;
  profileCpy.memState = 0;
  int mask[7] = { -5, 0, 3, 4, 3, 0, -5 };
  int maskCenter = 3;
  int maskLen = 7;
  long locMeanSignal;
  int inspectedSignals;

  if ( profile->glSum == NULL || profile->memState != 1 ) {
    status = -1;
    printf( "ERROR: extract_wells_glprofile(): Invalid argument 'glProfile->list == NULL || glprofile->memState != 1'.\n" );
  } else {
    // 1. produce a copy of the grey-level profile
    status = new_glProfile( &profileCpy, profile->length );
    if ( status == 0 )
      status = copy_glProfile( profile, &profileCpy );
    // 2. compute the convolution with the mask
    if ( status == 0 ) {
      printf( ">> computing convolution with well mask...\n" );
      for ( i=0; i < profileCpy.length-maskLen; i++ ) {
        profile->glSum[i + maskCenter] = 0;
        for ( ii = 0; ii < maskLen; ii++ )
          profile->glSum[i+maskCenter] += mask[ii] * profileCpy.glSum[i+ii];
      }
      for ( i=0; i < maskCenter; i++ )
        profile->glSum[i] = profile->glSum[maskLen];
      for ( i = profileCpy.length-maskLen; i < profileCpy.length; i++ )
        profile->glSum[i] = profile->glSum[profileCpy.length-maskLen - 1];
      printf( ">> OK\n" );
    }
    // 3. extract the local minima
    if ( status == 0 ) {
      printf( ">> extracting local minima\n" );
      status = extract_localMinima_glProfile( profile );
      if ( status == 0 )
        printf( ">> OK\n" );
    }
    // 4.a. throw out all positive signals
    for ( i=0; i < profile->length; i++ )
      if ( profile->glSum[i] > 0 )
        profile->glSum[i] = 0;
    // 4.b. throw out signal close to left and right edge
    for ( i=0; i < profile->length / 20; i++ )
      profile->glSum[i] = profile->glSum[profile->length - 1 - i] = 0;
    // 5. throw out all signals that are higher than a threshold calculated from the local mean of 9 signals
    printf( "i\tsignal\tinspSgnls locmeanSignal\n" );
    for ( i=0; i < profile->length; i++ ) {
      if ( profile->glSum[i] != 0 ) {
        printf( "%d\t%ld ", i, profile->glSum[i] );
        locMeanSignal = 0;
        inspectedSignals = 0;
        for ( ii = i+1; ii < profile->length && inspectedSignals < 9; ii++ )
          if ( profile->glSum[ii] != 0 ) {
            locMeanSignal += profile->glSum[ii];
            inspectedSignals++;
          }
        if ( inspectedSignals > 0 ) {
          locMeanSignal /= inspectedSignals;
          printf( "%d %ld\t", inspectedSignals, locMeanSignal );
          if ( profile->glSum[i] > (int)(locMeanSignal * 0.8) ) {
            printf( "out!" );
            profile->glSum[i] = 0;
          }
          printf( "\n" );
        }
      }
    }
    // 6. copy grey-level values back from copy to non-zero entries of profile
    for ( i=0; i < profile->length; i++ )
      if ( profile->glSum[i] != 0 )
        profile->glSum[i] = profileCpy.glSum[i];
    if ( delete_glProfile( &profileCpy ) != 0 ) {
      status = -1;
      printf( "ERROR: extract_wells_glProfile(): Failed to free 'profileCpy'.\n" );
    }
  }

  return status;
}



int get_meanPeakSpacing( glProfile_t *profile, float *peakSpacing ) {
  int status = 0;
  int i = 0, prev_i = 0;
  int threshold;
  int nSpaces;
  int sum;

  if ( profile->glSum == NULL || profile->memState != 1 ) {
    status = -1;
    printf( "ERROR: get_meanRasterSpacing(): Invalid argument 'profile->glSum == NULL || profile->memState != 1'.\n" );
  } else {
    threshold = 0;
    sum = 0;
    while ( prev_i == -1 && i < profile->length ) {
      if ( profile->glSum[i] > threshold )
        prev_i = i;
      i++;
    }
    if ( i == profile->length ) {
      status = -1;
      printf( "ERROR: get_meanRasterSpacing(): No peaks found in profile.\n" );
    } else {
      nSpaces = 0;
      for ( i = prev_i + 1; i < profile->length; i++ )
        if ( profile->glSum[i] > threshold ) {
          sum += i - prev_i;
          prev_i = i;
          nSpaces++;
        }
      if ( nSpaces == 0 ) {
        status = -1;
        printf( "ERROR: get_meanRasterSpacing(): Only one peak at i=%d in profile.\n", prev_i );
      }
    }
    if ( status == 0 )
      *peakSpacing = (int)((float)sum / nSpaces);
  }

  return status;
}



int get_peakSpacingHistogram( glProfile_t *glProfile, int peakSpacingHist[40], int *nPeaks ) {
  int status = 0;
  int threshold = 0;
  int i, prev_i;
  int dist;
  int n;

  if ( glProfile->glSum == NULL || glProfile->memState != 1 ) {
    status = -1;
    printf( "ERROR: get_peakSpacingHistogram(): Invalid argument 'glProfile->glSum == NULL || glProfile->memState != 1'.\n" );
  } else {
    for ( i=0; i<40; i++ )
      peakSpacingHist[i] = 0;
    prev_i = -1;
    i = 0;
    while ( prev_i == -1 && i < glProfile->length ) {
      if ( glProfile->glSum[i] > threshold )
        prev_i = i;
      i++;
    }
    if ( i == glProfile->length ) {
      status = -1;
      printf( "ERROR: get_PeakSpacingHistogram(): No peaks found in profile.\n" );
    } else {
      dist = 0;
      n = 1;
      for ( i = prev_i + 1; i < glProfile->length; i++ )
        if ( glProfile->glSum[i] > threshold ) {
          dist = i - prev_i;
          prev_i = i;
          n++;
          if ( dist < 40 )
            peakSpacingHist[dist]++;
          else
            printf( "WARNING: get_PeakSpacingHistogram(): Peak spacing too large 'dist > 40' --> ignoring this distance.\n" );
        }
      if ( n > 1 ) {
        *nPeaks = n;
      } else {
        status = -1;
        printf( "ERROR: get_PeakSpacingHistogram(): Only one peak found in profile.\n" );
      }
    }
  }

  return status;
}



int get_rasterSpacing( int spacingHist[40], int *rasterSpacing ) {
  int status = 0;
  int i;
  int regDist;
  int regDistCount;
  int distCount;
  regDist = regDistCount = distCount = 0;

  for ( i=3; i<40; i++ ) { // ignoring small spacings
    if ( spacingHist[i] > regDistCount ) {
      regDist = i;
      regDistCount = spacingHist[i];
    }
  }
  if ( regDist > 0 ) {
    printf( "> regular distance = %d (%d of %d)\n", regDist, regDistCount, distCount );
    *rasterSpacing = regDist;
  } else {
    if ( *rasterSpacing > 3 ) {
      printf( "WARNING: Could not determine regular distance of raster, using value from previous frame.\n" );
    } else {
      status = -1;
      printf( "ERROR: Could not determine regular distance of raster.\n" );
    }
  }

  return status;
}



int regularize_peakRaster( glProfile_t *glProfile, int rasterSpacing, int *nPeaks ) {
  int status = 0;
  int i, ii, i_prev, i_next, i_start, i_end;
  int threshold = 0;
  int dist;
  float fixDist;
  int nPeaksToFix;
  float effectiveMeanSpacing;
  int extractedRegDistances;
  int nAddedPeaks_left, nAddedPeaks_right, nAddedPeaks_middle;
  int nRemovedPeaks;
  int *regPeaks;
  int memState_regPeaks = 0;
  int nRegPeaks = 0;

  if ( glProfile->glSum == NULL || glProfile->memState != 1 ) {
    status = -1;
    printf( "ERROR: regularize_peakRaster(): Invalid argument 'glProfile->glSum == NULL || glProfile->memState != 1'.\n" );
  } else if ( glProfile->direction != 0 ) {
    status = -1;
    printf( "ERROR: regularize_peakRaster(): Invalid argument 'glProfile->direction != 0'.\n" );
  } else if ( rasterSpacing < 4 ) {
    status = -1;
    printf( "ERROR: Invalid argument 'rasterSpacing = %d < 4'.\n", rasterSpacing  );
  } else {
    i_prev = -1;
    i = 0;
    while ( i_prev == -1 && i < glProfile->length ) {
      if ( glProfile->glSum[i] > threshold )
        i_prev = i;
      i++;
    }
    if ( i_prev == -1 ) {
      *nPeaks = 0;
      printf( "WARNING: regularize_peakRaster(): First peak not found in profile. Skipping regularization of raster.\n" );
    } else {
      *nPeaks = 1;
    }
  }
  if ( status == 0  && *nPeaks > 0 ) {
    if ( (regPeaks = malloc( glProfile->length * sizeof(int) )) == NULL ) {
      status = -1;
      printf( "ERROR: regularize_peakRaster(): Failed to allocate memory for 'regPeaks'.\n" );
    } else {
      memState_regPeaks = 1;
      for ( i=0; i < glProfile->length; i++ )
        regPeaks[i] = 0;
    }
  }
  if ( status == 0 && *nPeaks > 0 ) {
    printf( "> looking for regular peaks, i.e. such spaced by rasterSpacing=%d, starting\n  from first peak detected at %d\n", rasterSpacing, i_prev );
    extractedRegDistances = 0;
    i = i_prev + 1;
    while ( i < glProfile->length ) {
      if ( glProfile->glSum[i] > threshold ) {
        dist = i - i_prev;
        if ( dist == rasterSpacing ) {
          regPeaks[i_prev] = regPeaks[i] = 1;
          extractedRegDistances++;
        }
        i_prev = i;
      }
      i++;
    }
    printf( "detected %d regular spaces between extracted peaks\n", extractedRegDistances );
    if ( extractedRegDistances == 0 ) {
      *nPeaks = 0;
      printf( "WARNING: No regular peaks found, skipping regularization of raster.\n" );
    }
  }
  if ( status == 0 && *nPeaks > 0 ) {
    i_prev = -1;
    dist = -1;
    nRemovedPeaks = 0;
    for ( i=0; i < glProfile->length; i++ )
      if ( regPeaks[i] == 1 ) {
        if ( i_prev == -1 ) {
          i_prev = i;
        } else {
          dist = i - i_prev;
          if ( dist != rasterSpacing && (dist < (3 * rasterSpacing) / 2) ) {
            regPeaks[i_prev] = regPeaks[i] = -1;
            nRemovedPeaks += 2;
          }
          i_prev = i;
        }
        nRegPeaks++;
      }
    printf( "removed %d of the extracted peaks because they formed an odd distance to neighboring peaks\n", nRemovedPeaks );
    nRegPeaks = 0;
    i_prev = -1;
    dist = -1;
    for ( i=0; i < glProfile->length; i++ )
      if ( regPeaks[i] == 1 ) {
        if ( i_prev == -1 ) {
          i_prev = i;
        } else {
          dist = i - i_prev;
          i_prev = i;
        }
        nRegPeaks++;
      }
    printf( "The number of well spaced extracted peaks is %d.\n", nRegPeaks );
  }
  if ( status == 0 && nRegPeaks > 0 ) {
    printf( "> Filling in missing peaks to form a complete regular raster.\n" );
    // left boundary region first
    for ( i=0; i < glProfile->length; i++ ) {
      if ( regPeaks[i] == 1 ) {
        i_prev = i;
        i = glProfile->length;
      }
    }
    printf( "leftmost extracted regular peak is at i=%d\n", i_prev );
    nAddedPeaks_left = 0;
    for ( i = i_prev-rasterSpacing; i>0; i -= rasterSpacing ) {
      regPeaks[i] = 2;
      nRegPeaks++;
      nAddedPeaks_left++;
    }
    printf( "Added %d peaks at left boundary.\n", nAddedPeaks_left );
    // right boundary region
    for ( i = glProfile->length; i > i_prev; i-- )
      if ( regPeaks[i] == 1 ) {
        i_next = i;
        i = i_prev;
      }
    printf( "rightmost extracted regular peak is at i=%d\n", i_next );
    nAddedPeaks_right = 0;
    for ( i = i_next+rasterSpacing; i < glProfile->length; i += rasterSpacing ) {
      regPeaks[i] = 2;
      nRegPeaks++;
      nAddedPeaks_right++;
    }
    printf( "Added %d peaks at right boundary.\n", nAddedPeaks_right );
    i_start = i_prev;
    i_end = i_next;
    printf( "now fixing missing peaks in gaps located between %d and %d\n", i_start, i_end );
    nAddedPeaks_middle = 0;
    for ( i = i_start; i < i_end; i += rasterSpacing ) {
      if ( regPeaks[i] < 1 ) {
        ii = i + 1;
        while (  regPeaks[ii] < 1 && ii <= i_end )
          ii++;
        i_next = ii;
        dist = (i_next-i_prev);
        printf( "  gap of %d pixels (from %d to %d)\t", dist, i_prev, i_next );
        nPeaksToFix = (int)(((float)dist) / rasterSpacing + 0.5) - 1;
        fixDist = (float)dist / (nPeaksToFix+1);
        printf( "--> npeaksToFix = %d, spaced by %f :\n\t\t", nPeaksToFix, fixDist );
        for ( ii = 1; ii <= nPeaksToFix; ii++ ) {
          printf( "|%f ", i_prev+ii*fixDist );
          regPeaks[i_prev + (int)(ii * fixDist + 0.5)] = 3;
          nAddedPeaks_middle++;
        }
        printf( "\n" );
        i_prev = i_prev + (int)(ii * fixDist + 0.5);
        i = i_next;
      } else {
        i_prev = i;
      }
    }
    printf( "Added %d peaks in the middle to fill gaps.\n", nAddedPeaks_middle );
  }
  if ( status == 0 && nRegPeaks > 0 ) {
    nRegPeaks = 0;
    i_prev = i_next = -1;
    dist = -1;
    for ( i=0; i < glProfile->length; i++ ) {
      if ( regPeaks[i] > 0 ) {
        nRegPeaks++;
        if ( i_prev == -1 ) {
          i_prev = i;
          ii = i;
        } else {
          dist = i - ii;
          ii = i;
        }
        if ( i > i_next )
          i_next = i;
        glProfile->glSum[i] = regPeaks[i];
      } else {
        glProfile->glSum[i] = 0;
      }
    }
    dist = i_next -i_prev;
    effectiveMeanSpacing = (float)dist / (nRegPeaks - 1);
    printf( "nRegPeaks = %d, total distance = %d, effective mean spacing = %f\n", nRegPeaks, dist, effectiveMeanSpacing );
    *nPeaks = nRegPeaks;
  }
  if ( memState_regPeaks != 0 )
    free( regPeaks );

  return status;
}



int relax_regPeakRaster( glProfile_t *glProfile, glProfile_t *regPeakProfile ) {
  int status = 0;
  int i;
  int maxDist = 4;
  printf( "> maxDist = %d\n", maxDist );
  int dist;
  int glBuf;
  int shift;

  if ( glProfile->glSum == NULL || glProfile->memState != 1 ) {
    status = -1;
    printf( "ERROR: relax_regPeakRaster(): Invalid argument 'glProfile->glSum == NULL || glProfile->memState != 1'.\n" );
  }
  if ( regPeakProfile->glSum == NULL || regPeakProfile->memState != 1 ) {
    status = -1;
    printf( "ERROR: relax_regPeakRaster(): Invalid argument 'regPeakProfile->glSum == NULL || regPeakProfile->memState != 1'.\n" );
  }
  if ( glProfile->length != regPeakProfile->length ) {
    status = -1;
    printf( "ERROR: relax_regPeakRaster(): Inconsistent profile length ' glProfile->length != regPeakProfile->length'.\n" );
  } else {
    for ( i = maxDist; i < regPeakProfile->length-maxDist; i++ )
      if ( regPeakProfile->glSum[i] > 0 ) {
        glBuf =  glProfile->glSum[i];
        shift = 0;
        for ( dist = -maxDist; dist <= maxDist; dist++ ) {
          if ( glProfile->glSum[i + dist] < glBuf ) {
            shift = dist;
            glBuf = glProfile->glSum[i + dist];
          }
        }
        if ( shift != 0 ) {
          regPeakProfile->glSum[i+shift] = regPeakProfile->glSum[i];
          regPeakProfile->glSum[i] *= -1;
          printf( "> peak at x-position %d was shifted %d pixels\n", i, shift );
        }
      }
  }

  return status;
}



int count_peaks_glProfile( glProfile_t *profile, int *nPeaks ) {
  int status = 0;
  int i;
  int threshold = 0;
  int n = 0;

  if ( profile->glSum == NULL || profile->memState != 1 ) {
    status = -1;
    printf( "ERROR: count_peaks_glProfile(): Invalid argument 'profile->glSum == NULL || profile->memState != 1'.\n" );
  }
  for ( i=0; i < profile->length; i++ )
    if ( profile->glSum[i] > threshold )
      n++;
  *nPeaks = n;

  return status;
}



int get_peakPositions( glProfile_t *profile, positionList_t *peakPos ) {
  int status = 0;
  int i = 0;
  int threshold = 0;
  int cooOffset;
  int peakNo;
  int nIgnoredPeaks;

  if ( profile->glSum == NULL || profile->memState != 1 ) {
    status = -1;
    printf( "ERROR: store_peakPositions(): Invalid argument 'profile->glSum == NULL || profile->memState != 1'.\n" );
  } else if ( peakPos->list == NULL || peakPos->memState != 1 ) {
    status = -1;
    printf( "ERROR: store_peakPositions(): Invalid argument 'peakPos->list == NULL || peakPos->memState != 1'.\n" );
  } else if ( profile->direction!=0 && profile->direction != 1 ) {
    status = -1;
    printf( "ERROR: store_peakPositions(): Invalid argument 'profile->direction != 0 && profile->direction != 1'.\n" );
  } else {
    if ( profile->direction == 0 ) {
      cooOffset = profile->p.x;
    } else if ( profile->direction == 1 ) {
      cooOffset = profile->p.y;
    }
    peakNo = 0;
    printf( "Looking for %d peaks along a profile of %d pixels length.\n", peakPos->length, profile->length );
    while ( peakNo < peakPos->length && i < profile->length ) {
      if ( profile->glSum[i] > threshold ) {
        peakPos->list[peakNo] = i+cooOffset;
        peakNo++;
      }
      i++;
    }
    nIgnoredPeaks = 0;
    if ( i < profile->length ) {
      while ( i < profile->length ) {
        if ( profile->glSum[i] > threshold )
          nIgnoredPeaks++;
        i++;
      }
      if ( nIgnoredPeaks > 0 ) {
        printf( "WARNING: get_peakPositions(): Peak position list too short, ignoring %d peaks.\n", nIgnoredPeaks );
      }
    }
    if ( peakNo < peakPos->length ) {
      printf ( "WARNING: get_peakPositions(): Peak position list too long: 'peakNo=%d < peakPos->length=%d'.\n", peakNo, peakPos->length );
      for ( i = peakNo; i < peakPos->length; i++ ) {
        peakPos->list[i] = -1;
      }
    }
  }

  return status;
}



int correct_oddPeakPositions( positionList_t *peakPos ) {
  int status = 0;
  int i;
  float meanPeakSpacing;
  float *peakEnergy = NULL;
  int peakEnergy_memState = 0;
  int dist_l, dist_r;
  int diff_lr;

  meanPeakSpacing = (float)(peakPos->list[peakPos->length - 1] - peakPos->list[0]) / (peakPos->length - 1);
  printf( "meanPeakSpacing = %f\n", meanPeakSpacing );
  for ( i=1; i < peakPos->length - 1; i++ ) {
    dist_l = peakPos->list[i] - peakPos->list[i-1];
    dist_r = peakPos->list[i+1] - peakPos->list[i];
    if ( (dist_l + dist_r - 2 * meanPeakSpacing) < 1.5 ) {
      diff_lr = dist_l - dist_r;
      if ( diff_lr < 0 )
        diff_lr *= -1;
      if ( diff_lr > 2 ) {
        printf( "Raster line at xPos = %d is odd one out of its left and right neighbors.\n", peakPos->list[i] );
        printf( "It is put at xPos' = %d and peak with id %d is jumped over.\n", (int)((peakPos->list[i+1] + peakPos->list[i-1]) / 2.0 + 0.5), i+1 );
        peakPos->list[i] = (int)((peakPos->list[i+1] + peakPos->list[i-1]) / 2.0 + 0.5);
        i++;
      }
    }
  }
  if ( (peakEnergy = malloc( (peakPos->length) * sizeof(float) )) == NULL ) {
    status = -1;
    printf( "ERROR: correct_oddPeakPositions(): Could not allocate memory for 'peakEnergy'.\n" );
  } else {
    peakEnergy_memState = 1;
  }
  if ( peakEnergy_memState == 1 )
    free( peakEnergy );

  return status;
}



// reconstructing the colors

int reconstruct_colorFrame( glImage_t *glScan, positionList_t *rasterPos, rgbImage_t *rgbFrame ) {
  int status = 0;
  int c, i, s;
  int xCol, yCol;
  int x, y, yy;
  int enveloppe[1] = {1};
  int enveloppeLength = 1;
  int enveloppeCenter = 0;
  int yBinSize;
  int rasterOffset;
  float chSpace;
  float colTrsf[3][3] = {{1.021277, -0.148936, 0.021277}, {-0.148936, 1.042553, -0.148936}, {0.021277, -0.148936, 1.021277}};
  float col[3];
  int gls[3];
  int xPos;
  int rgbFrameWidth;
 
  if ( glScan->img == NULL || glScan->memState != 1 ) {
    status = -1;
    printf( "ERROR: reconstruct_colorFrame(): Invalid argument 'glScan->img == NULL || glScan->memState != 1'.\n" );
  } else if ( rasterPos->list == NULL || rasterPos->memState != 1 ) {
    status = -1;
    printf( "ERROR: reconstruct_colorFrame(): Invalid argument 'rasterPos->list == NULL || rasterPos->memState != 1'.\n" );
  } else if ( rgbFrame->img == NULL || rgbFrame->memState != 1 ) {
    status = -1;
    printf( "ERROR: reconstruct_colorFrame(): Invalid argument 'rgbFrame->img == NULL || rgbFrame->memState != 1'.\n" );
  } else if ( rgbFrame->width != rasterPos->length - 1 ) {
    status = -1;
    printf( "ERROR: reconstruct_colorFrame(): Incompatible arguments 'rgbFrame->width != rasterPos->length - 1'.\n" );
  } else {
    rasterOffset = 0;
    yBinSize = glScan->height / rgbFrame->height;
    printf( "yBinSize = %d\n", yBinSize );
    rgbFrameWidth = rgbFrame->width - 1;
    printf( "> using 'rgbFrameWidth' = %d to mirror frame horizontally\n", rgbFrameWidth );
  }
  if ( status == 0 ) {
    for ( xCol = 0; xCol < rgbFrame->width; xCol++ ) {
      x = rasterPos->list[xCol];
      chSpace = (rasterPos->list[xCol + 1] - x) / 4.0;
      for ( yCol = 0; yCol < rgbFrame->height; yCol++ ) {
        y = yBinSize * yCol;
        for ( i=0; i<3; i++ ) {
          gls[i] = 0;
          xPos = x+(int)((i+1)*chSpace);
          for ( yy = 0; yy < yBinSize; yy++ )
            for ( s=0; s < enveloppeLength; s++ )
              gls[i] += enveloppe[s] * (glScan->img[y + yy][xPos + s - enveloppeCenter]);
        }
        for ( c = 0; c < 3; c++ ) {
          col[c] = 0;
          for ( i=0; i<3; i++ )
            col[c] += colTrsf[c][i]*gls[i];
          if ( col[c] < 0 )
            rgbFrame->img[c][yCol][rgbFrameWidth - xCol] = 0;
          else
            rgbFrame->img[c][yCol][rgbFrameWidth - xCol] = (int)(col[c]);
        }
      }
    }
  }

  return status;
}



int reconstructInterpolate_colorFrame( glImage_t *glScan, positionList_t *rasterPos, rgbImage_t *rgbFrame ) {
  int status = 0;
  int c, i, s;
  int xCol, yCol;
  int x, y, yy;
  int rgbFrameWidth;
  float buf;
  int enveloppe[1] = {1};  //[3] = {1,2,1}; [5] = {1,3,4,3,1}; [7] = {1,2,7,8,7,2,1};
  int enveloppeLength = 1;
  int enveloppeCenter = 0;
  int yBinSize;
  int rasterOffset;
  float chSpace;
  float relD[3];
  int intDir;
  float colTrsf[3][3] = {{1.021277, -0.148936, 0.021277}, {-0.148936, 1.042553, -0.148936}, {0.021277, -0.148936, 1.021277}};
  float col[3];
  int gls[3];
  int xPos;

  // determine how many lines are to be summed up to compute the color of a pixel in the rgbFrame
  if ( glScan->img == NULL || glScan->memState != 1 ) {
    status = -1;
    printf( "ERROR: reconstructInterpolate_colorFrame(): Invalid argument 'glScan->img == NULL || glScan->memState != 1'.\n" );
  } else if ( rasterPos->list == NULL || rasterPos->memState != 1 ) {
    status = -1;
    printf( "ERROR: reconstructInterpolate_colorFrame(): Invalid argument 'rasterPos->list == NULL || rasterPos->memState != 1'.\n" );
  } else if ( rgbFrame->img == NULL || rgbFrame->memState != 1 ) {
    status = -1;
    printf( "ERROR: reconstructInterpolate_colorFrame(): Invalid argument 'rgbFrame->img == NULL || rgbFrame->memState != 1'.\n" );
  } else if ( rgbFrame->width != 2*(rasterPos->length - 1) - 1 ) {
    status = -1;
    printf( "ERROR: reconstructInterpolate_colorFrame(): Incompatible arguments 'rgbFrame->width != 2*(rasterPos->length-1)-2'.\n" );
  } else {
    rasterOffset = 0;
    yBinSize = glScan->height / rgbFrame->height;
    printf( "> yBinSize = %d\n", yBinSize );
    rgbFrameWidth = rgbFrame->width-1;
    printf( "> using 'rgbFrameWidth' = %d to mirror frame horizontally\n", rgbFrameWidth );
  }
  if ( status == 0 ) {
    for ( xCol = 0; xCol < rgbFrame->width; xCol += 2 ) {
      x = rasterPos->list[xCol/2];
      chSpace = (rasterPos->list[xCol / 2 + 1] - x) / 4.0;
      for ( yCol = 0; yCol < rgbFrame->height; yCol++ ) {
        y = yBinSize*yCol;
        for ( i=0; i<3; i++ ) {
          gls[i] = 0;
          xPos = x+(int)((i+1)*chSpace);
          for ( yy = 0; yy < yBinSize; yy++ )
            for ( s=0; s < enveloppeLength; s++ )
              gls[i] += enveloppe[s] * (glScan->img[y + yy][xPos + s - enveloppeCenter]);
        }
        for ( c=0; c<3; c++ ) {
          col[c] = 0;
          for ( i=0; i<3; i++ )
            col[c] += colTrsf[c][i] * gls[i];
          if ( col[c] < 0 )
            rgbFrame->img[c][yCol][rgbFrameWidth - xCol] = 0;
          else
            rgbFrame->img[c][yCol][rgbFrameWidth - xCol] = (int)(col[c]);
        }
      }
    }
    for ( c=0; c<3; c++ ) {
      for ( xCol = 1; xCol < rgbFrame->width - 1; xCol += 2 ) {
        rgbFrame->img[c][0][xCol] = (int)((rgbFrame->img[c][0][xCol - 1]+rgbFrame->img[c][0][xCol + 1]) / 2.0 + 0.5);
        rgbFrame->img[c][rgbFrame->height-1][xCol] = (int)((rgbFrame->img[c][rgbFrame->height - 1][xCol - 1]+rgbFrame->img[c][rgbFrame->height - 1][xCol + 1]) / 2.0 + 0.5);
      }
    }
    for ( yCol = 1; yCol < rgbFrame->height - 1; yCol++ ) {
      for ( xCol = 1; xCol < rgbFrame->width - 1; xCol += 2 ) {
        relD[0] = relD[1] = relD[2] = 0;
        for ( c = 0; c < 3; c++ ) {
          buf = ((float)(rgbFrame->img[c][yCol][xCol - 1] - rgbFrame->img[c][yCol][xCol + 1]))/(rgbFrame->img[c][yCol][xCol - 1] + rgbFrame->img[c][yCol][xCol + 1]);
          if ( buf < 0 )
            relD[0] -= buf;
          else
            relD[0] += buf;
          buf = ((float)(rgbFrame->img[c][yCol - 1][xCol - 1] - rgbFrame->img[c][yCol + 1][xCol + 1]))/(rgbFrame->img[c][yCol - 1][xCol - 1] + rgbFrame->img[c][yCol + 1][xCol + 1]);
          if ( buf < 0 )
            relD[1] -= buf;
          else
            relD[1] += buf;
          buf = ((float)(rgbFrame->img[c][yCol + 1][xCol - 1] - rgbFrame->img[c][yCol - 1][xCol + 1]))/(rgbFrame->img[c][yCol + 1][xCol - 1] + rgbFrame->img[c][yCol - 1][xCol + 1]);
          if ( buf < 0 )
            relD[2] -= buf;
          else
            relD[2] += buf;
        }
        intDir = 0;
        if ( relD[1] < relD[0] ) {
          intDir = 1;
          if ( relD[2] < relD[1] )
            intDir = 2;
        } else if ( relD[2] < relD[0] ) {
          intDir = 2;
        }
        if ( intDir == 0 ) {
          for ( c = 0; c < 3; c++ )
            rgbFrame->img[c][yCol][xCol] = (int)((rgbFrame->img[c][yCol][xCol - 1]+rgbFrame->img[c][yCol][xCol + 1]) / 2.0 + 0.5);
        } else if ( intDir == 1 )
          for ( c = 0; c < 3; c++ ) {
            rgbFrame->img[c][yCol][xCol] = (int)((rgbFrame->img[c][yCol - 1][xCol - 1]+rgbFrame->img[c][yCol + 1][xCol + 1]) / 2.0 + 0.5);
        } else {
          for ( c = 0; c < 3; c++ )
            rgbFrame->img[c][yCol][xCol] = (int)((rgbFrame->img[c][yCol + 1][xCol - 1]+rgbFrame->img[c][yCol - 1][xCol + 1]) / 2.0 + 0.5);
        }
      }
    }
  }

  return status;
}



int pickColor_interactively( glImage_t *glScan, positionList_t *rasterPos ) {
  int status = 0;
  int c, i;
  int r, g, b;
  int rgbRasterWidth, rgbRasterHeight;
  int rgbX, rgbY;
  int yBinSize;
  int glCurve[32];
  int x, y, xx, yy;
  int count;
  int dist;
  int backgroundGl;
  float colTrsf[3][3] = {{1.021277, -0.148936, 0.021277}, {-0.148936, 1.042553, -0.148936}, {0.021277, -0.148936, 1.021277}};
  float col[3];
  int glPoint[3];

  if ( glScan->img == NULL || glScan->memState != 1 ) {
    status = -1;
    printf( "ERROR: pickColor_interactively(): Invalid argument 'glScan->img == NULL || glScan->memState != 1'.\n" );
  } else if ( rasterPos->list == NULL || rasterPos->memState != 1 ) {
    status = -1;
    printf( "ERROR: pickColor_interactively(): Invalid argument 'rasterPos->list == NULL || rasterPos->memState != 1'.\n" );
  } else {
    rgbRasterWidth = rasterPos->length;
    rgbRasterHeight = (glScan->height * rgbRasterWidth) / (glScan->width);
    yBinSize = (glScan->height) / rgbRasterHeight;
    printf( "\n______ interactive mode _______\n" );
    printf( "dimensions of RGB frame: %d x %d\n", rgbRasterWidth, rgbRasterHeight );
    printf( "yBinSize = %d, xBinSize = dist between adjacent raster lines\n", yBinSize );
    printf( "\n" );
    printf( "Enter coordinates within the above reported range to see:\n" );
    printf( "  1. the grey-level profile and\n" );
    printf( "  2. get the color reconstructed therefrom.\n" );
    printf( "To exit enter '-1 -1' / three times in a row coordinates outside admissible range will force exit as will non int input.\n" );
    backgroundGl = 1000;
    printf( "bkgrnd = %d\n", backgroundGl );
    rgbX = rgbY = -2;
    count = 0;
    while ( !(rgbX == -1 && rgbY == -1) && count < 3 ) {
      printf( "rgbX = " );
      scanf( "%d", &rgbX );
      printf( "rgbY = " );
      scanf( "%d", &rgbY );
      if ( !(rgbX < rgbRasterWidth) || !(rgbY < rgbRasterHeight) ) {
        count++;
        printf( "position out of RRGBBG frame\n" );
        printf( "Enter coordinates within the above reported range to see:\n" );
        printf( "  1. the grey-level profile and\n" );
        printf( "  2. get the color reconstructed therefrom.\n" );
        printf( "To exit enter '-1 -1'.\n" );
      } else if ( !(rgbX > -1) || !(rgbY > -1) ) {
        count++;
        printf( "position out of RGB frame\n" );
        printf( "Enter coordinates within the above reported range to see:\n" );
        printf( "  1. the grey-level profile and\n" );
        printf( "  2. get the color reconstructed therefrom.\n" );
        printf( "To exit enter '-1 -1'.\n" );
      } else if ( rgbX != -1 && rgbY != -1 ) {
        x = rasterPos->list[rgbX];
        y = rgbY*yBinSize;
        printf( "> (%d,%d)rgbFrame <-> (%d,%d)glScan\n", rgbX, rgbY, x, y );
        dist = rasterPos->list[rgbX+1] - x + 1;
        if ( dist < 32 ) {
          for ( xx = 0; xx < dist; xx++ ) {
            glCurve[xx] = 0;
            for ( yy = 0; yy < yBinSize; yy++ )
              glCurve[xx] += glScan->img[y + yy][x + xx];
          }
          printf( "Here is the grey-level curve:\n" );
          for ( xx = 0; xx < dist; xx++ )
            printf( "% 6d ", glCurve[xx] );
          printf( "\n" );
          for ( c=0; c<3; c++ )
            glPoint[c] = glCurve[(c + 1) * dist / 4] - backgroundGl;
          for ( i=0; i<3; i++ )
            for ( c=0; c<3; c++ )
              col[i] += colTrsf[i][c] * glPoint[c];
          r = (int)col[0];
          b = (int)col[1];
          g = (int)col[2];
          printf( "|r'|g'|b'| = |%d|%d|%d|\n", r, g, b );
        } else {
          printf( "Distance to next raster too large, ignoring color pick request.\n" );
        }
      }
    }
  }

  return status;
}



// rather geometry stuff...

// localising the lens array

int new_positionList( positionList_t *positions, int length ) {
  int status = 0;

  if ( positions->list != NULL || positions->memState != 0 ) {
    status = -1;
    printf( "ERROR: new_positionList(): Invalid argument 'positions->list != NULL || positions->memState != 0'.\n" );
  } else if ( length < 0 ) {
    status = -1;
    printf( "ERROR: new_positionList(): Invalid argument 'length < 0'.\n" );
  } else {
    positions->length = length;
    if ( (positions->list = malloc( length * sizeof(int) )) == NULL ) {
      status = -1;
      printf( "ERROR: new_positionList(): Failed to allocate memory for 'positions->list'.\n" );
    }
    if ( status == 0 )
      positions->memState = 1;
    else
      positions->memState = -1;
  }

  return status;
}



int delete_positionList( positionList_t *positions ) {
  int status = 0;

  if ( positions->list == NULL || positions->memState == 0 ) {
    status = -1;
    printf( "ERROR: delete_positionList(): Cannot free memory 'positions->list == NULL || positions->memState == 0'.\n" );
  } else if ( positions->length < 0 ) {
    status = -1;
    printf( "ERROR: delete_positionList(): Invalid argument 'positions->length < 0'.\n" );
  } else {
    free( positions->list );
    positions->list = NULL;
    positions->memState = 0;
  }

  return status;
}



// drawing into image

int draw_verticalLines_rgbImage( rgbImage_t *rgbImage, positionList_t *positions, int color[3] ) {
  int status = 0;
  int c, i;
  int x, y, x_max, y_max;

  if ( rgbImage->img == NULL || rgbImage->memState != 1 ) {
    status = -1;
    printf( "ERROR: draw_verticalLines_rgbImage(): Invalid argument 'rgbImage->img == NULL || rgbImage->memState != 1'.\n" );
  } else if ( positions->list == NULL || positions->memState != 1 ) {
    status = -1;
    printf( "ERROR: draw_verticalLines_rgbImage(): Invalid argument 'positions->list == NULL || positions->memState != 1'.\n" );
  } else if ( color[0] < 0 || color[0] > 255 || color[1] < 0 || color[1] > 255 || color[2] < 0 || color[2] > 255 ) {
    status = -1;
    printf( "ERROR: draw_verticalLines_rgbImage(): Invalid argument 'color[0] < 0 || color[0] > 255 || color[1] < 0 || color[1] > 255 || color[2] < 0 || color[2] > 255'.\n" );
  } else {
    x_max = rgbImage->width - 1;
    y_max = rgbImage->height - 1;
    for ( i=0; i < positions->length && status == 0; i++ ) {
      x = positions->list[i];
      if ( x >= 0 && x <= x_max )
        for ( y=0; y <= y_max; y++ )
          for ( c=0; c<3; c++ )
            rgbImage->img[c][y][x] = color[c];
      else
        printf( "WARNING: draw_verticalLines_rgbImage(): Skipping drawing of line, because line is outside of image x=%d not within 'x_min=0, x_max=%d'.\n", x, x_max );
    }
  }

  return status;
}



int draw_raster_rgbImage( rgbImage_t *rgbImage, glProfile_t *profile, positionList_t *positions ) {
  int status = 0;
  int i;
  int x, y, x_max, y_max;

  if ( rgbImage->img == NULL || rgbImage->memState != 1 ) {
    status = -1;
    printf( "ERROR: draw_raster_rgbImage(): Invalid argument 'rgbImage->img == NULL || rgbImage->memState != 1'.\n" );
  } else if ( positions->list == NULL || positions->memState != 1 ) {
    status = -1;
    printf( "ERROR: draw_raster_rgbImage(): Invalid argument 'positions->list == NULL || positions->memState != 1'.\n" );
  } else if ( profile->glSum == NULL || profile->memState != 1 ) {
    status = -1;
    printf( "ERROR: draw_raster_rgbImage(): Invalid argument 'profile->glSum == NULL || profile->memState != 1'.\n" );
  } else {
    printf( "Wanna draw lines now with the color set by the kind of the line, i.e. extracted or reconstructed.\n" );
    x_max = rgbImage->width - 1;
    y_max = rgbImage->height - 1;
    for ( i=0; i < positions->length && status == 0; i++ ) {
      x = positions->list[i];
      if ( x >= 0 && x <= x_max )
        if ( profile->glSum[x] == 1  ) {
          for ( y=0; y <= y_max; y++ ) {
            rgbImage->img[0][y][x] /= 3;
            rgbImage->img[1][y][x] /= 3;
            rgbImage->img[2][y][x] = 255;
          }
        } else if ( profile->glSum[x] == 2  ) {
          for ( y=0; y <= y_max; y++ ) {
            rgbImage->img[0][y][x] = 255;
            rgbImage->img[1][y][x] /= 3;
            rgbImage->img[2][y][x] = 255;
          }
        } else if ( profile->glSum[x] == 3 ) {
          for ( y=0; y <= y_max; y++ ) {
            rgbImage->img[0][y][x] /= 3;
            rgbImage->img[1][y][x] = 255;
            rgbImage->img[2][y][x] /= 3;
          }
        } else {
          printf( "WARNING: draw_raster_rgbImage(): Missing line: 'profile->glSum[%d] = %ld'.\n", x, profile->glSum[x] );
        }
      else
        printf( "WARNING: draw_raster_rgbImage(): Skipping drawing of line, because line is outside of image x=%d not within 'x_min=0, x_max=%d'.\n", x, x_max );
    }
  }
 
  return status;
}
