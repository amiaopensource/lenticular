
// Copyright (C) 2012 Joakim Reuteler

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// _______________________________________________________________________ 



// imageHandling.c
// - implementation of allocation and freeing of memory for gray level and rgb images
// - implementation of reading and writing TIFF images
//


#include "imageHandling.h"

#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>




// **************************************
// *** gray level and rgb image types ***
// **************************************

// new gray level image
int new_glImage( glImage_t *glImage, int width, int height ) {
    int status = 0;
    int j;
    
    if ( glImage->img!=NULL || glImage->memState!=0 ) {
        status = -1;
        printf( "ERROR: new_glImage(): Invalid arguiment 'glImage->img!=NULL || glImage->memState!=0'.\n" );
    } else if ( width<1 || height<1 ) {        
        status = -1;
        printf( "ERROR: new_glImage(): Invalid argumment 'width<1 || heigth<1'\n" );
    } 
    
    if ( status == 0 ) {
        glImage->width = width;
        glImage->height = height;
        
        glImage->img = malloc( (long)(glImage->height)*sizeof(int*) );
        if ( glImage->img == NULL ) {
            status = -1;
            printf( "ERROR: new_glImage(): Failed to allocate memory for glImage->img\n" ); 
        }
        for ( j=0; j<glImage->height && status==0; j++ ){
            glImage->img[j] = malloc( (long)(glImage->width)*sizeof(int) );
            if ( glImage->img[j] == NULL ) {
                status = -1;
                printf( "ERROR: new_glImage(): Failed to allocate memory for glImage->img[%d]\n", j );
            }
        }
    }
    
    if ( status == 0 ) {
        glImage->memState = 1;
    } else {
        glImage->memState = -1;
    }
    
    return status;
}

// delete gray level image
int delete_glImage( glImage_t *glImage ){
    int status = 0;
    int j;
  
    if ( glImage->img==NULL || glImage->memState==0 ){
        status = -1;
        printf( "ERROR: delete_glImage(): Nothing to do 'glImage->img==NULL || glImage->memState==0'.\n" ); 
    } else if ( glImage->width<1 || glImage->height<1 ) {
        status = -1;
        printf( "ERROR: delete_glImage(): Invalid argument 'glImage->width<1 || glImage->height<1'.\n" ); 
    } 
    
    if ( status == 0 ) {
        for ( j=0; j<glImage->height; j++ ){
            free( glImage->img[j] ); 
        }
        free( glImage->img );
        glImage->img = NULL;
        glImage->memState = 0;
    }
    
    return status;
}

// new rgb image
int new_rgbImage( rgbImage_t *rgbImage, int width, int height ){
    int status = 0;
    int c,j;
    
    if ( rgbImage->img!=NULL || rgbImage->memState!=0 ) {
        status = -1;
        printf( "ERROR: new_bitstore_rgbImage(): Invalid argument 'rgbImage->img!=NULL || rgbImage->memState!=0'.\n" );
    } else if ( width<1 || height<1 ) {        
        status = -1;
        printf( "ERROR: Won't allocate memory because width<1 || heigth<1\n" );
    } 
    
    if ( status == 0 ) {
        rgbImage->width = width;
        rgbImage->height = height;
        
        rgbImage->img = malloc( (long)(3)*sizeof(int**) );
        if ( rgbImage->img == NULL ) {
            status = -1;
            printf( "ERROR: new_bitstore_rgbImage(): Failed to allocate memory for rgbImage->img\n" ); 
        }
        for ( c=0; c<3 && status==0; c++ ) {
            rgbImage->img[c] = malloc( (long)(rgbImage->height)*sizeof(int*) );
            if ( rgbImage->img[c] == NULL ) {
                printf( "ERROR: new_bitstore_rgbImage(): Failed to allocate memory for rgbImage->img[%d]\n", c );
            }
            for ( j=0; j<rgbImage->height && status==0; j++ ){
                rgbImage->img[c][j] = malloc( (long)(rgbImage->width)*sizeof(int) );
                if ( rgbImage->img[c][j] == NULL ) {
                    status = -1;
                    printf( "ERROR: new_bitstore_rgbImage():Failed to allocate memory for rgbImage->img[%d][%d]\n", c, j );
                }
            }
        }
    }
    
    if ( status == 0 ) {
        rgbImage->memState = 1;
    } else {
        rgbImage->memState = -1;
    }
    
    return status;
}

// delete rgb image
int delete_rgbImage( rgbImage_t *rgbImage ){
    int status = 0;
    int c,j; 
    //
    if ( rgbImage->img==NULL && rgbImage->memState==0 ){
        status = -1;
        printf( "ERROR: delete_rgbImage(): Nothing to do 'rgbImage->img==NULL && rgbImage->memState==0'.\n" ); 
    } else if ( rgbImage->width<1 || rgbImage->height<1 ) {
        status = -1;
        printf( "ERROR: delete_rgbImage():Invalid argument 'rgbImage->width<1 || rgbImage->height<1'.\n" ); 
    } else if ( rgbImage->memState != 1 ) {
        status = -1;
        printf( "ERROR: delete_rgbImage(): Invalid argument 'rgbImage->memState!=1'.\n" );
    }

    if ( status == 0 ) {
        for ( c=0; c<3; c++ ){
            for ( j=0; j<rgbImage->height; j++ ){
                free( rgbImage->img[c][j] );
            }
            free( rgbImage->img[c] );
        }
        free( rgbImage->img );
        rgbImage->img = NULL;
        rgbImage->memState = 0;
    }
    
    return status;
}





// **********************
// *** handling TIFFs ***
// **********************

// check  TIFF 
int check_TIFF( char *fileName, short *spp, short *bps, int *width, int *height ){
    int status = 0;
    *spp = *bps = -1;
    TIFF *inTIFF;
    inTIFF = NULL;
    
    if( (inTIFF = TIFFOpen( fileName, "r" )) == NULL ){
        status = -1;
        printf( "ERROR: check_TIFF(): Failed to open '%s'.\n", fileName );
    }
    
    if ( status == 0 ){
        TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, spp );
        TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, bps );
        TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, width );
        TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, height );
    }
    if ( *spp==-1 || *bps==-1 ){
        status = -1;
        printf( "ERROR: Failed to read TIFF-tags in '%s'\n", fileName );
    }    
    if( inTIFF != NULL ){
        TIFFClose( inTIFF );
    }
    return status;
}

// read gray level TIFF into memory
int read_glTIFF( char *inputTIFFname, glImage_t *glImage ){
    int status = 0;
    int i, j;
    unsigned char *lineBuffer;
    int width, height;
    short spp, bps;
    TIFF *inTIFF;
    inTIFF = NULL;
    
    if ( glImage->img==NULL || glImage->memState!=1 ){
        status = -1;
        printf( "ERROR: Invalid argument 'glImage->img==NULL || glImage->memState!=1'.\n" );
    }
    if ( status == 0 ){
        if ( (inTIFF = TIFFOpen( inputTIFFname, "r" )) == NULL ){
            status = -1;
            printf( "ERROR: failed to open TIFF.\n" );
        }
    }
    
    if ( status == 0 ){
        TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, &spp );
        TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
        TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, &width );
        TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, &height );
        if( spp!=1 || bps!=8 || glImage->width!=width || glImage->height!=height ){
            status = -1;
            printf( "ERROR: Can't read '%s' because input parameters are inconsistent.\n", inputTIFFname );
            printf( "/tActual parameters  : spp=%d, bps=%d, width=%d, height%d\n", spp, bps, width, height );
            printf( "/tExpected parameters: spp=%d, bps=%d, width=%d, heigth%d\n", 1, 8, glImage->width, glImage->height );
        }
    }
    
    
    if ( status == 0 ){
        lineBuffer = malloc( (long)(glImage->width)*sizeof(unsigned char) );
        if ( lineBuffer == NULL ) {
            status = -1;
            printf( "ERROR: Failed to allocate memory for lineBuffer \n" );   
        } else {
            for ( j=0; j<glImage->height; j++ ){
                if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ){
                    printf( "ERROR: Failed to read scanline %d from %s.\n", j, inputTIFFname );
                    status = -1;
                } else {
                    for( i=0; i<glImage->width; i++ ){
                        glImage->img[j][i] = (int)lineBuffer[i];
                    }
                }
            }
            free( lineBuffer );
        }
    }
    
    if ( inTIFF != NULL ){
        TIFFClose( inTIFF );
    }
    
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
    
    if ( glImage->img==NULL || glImage->memState!=1 ){
        status = -1;
        printf( "ERROR: read_16bitTIFF_glImage(): Invalid argument 'glImage->img==NULL || glImage->memState!=1'.\n" );
    }
    if ( status == 0 ){
        if ( (inTIFF = TIFFOpen( TIFFname, "r" )) == NULL ){
            status = -1;
            printf( "ERROR: failed to open TIFF.\n" );
        }
    }
    
    if ( status == 0 ){
        TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, &spp );
        TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
        TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, &width );
        TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, &height );
        if( spp!=1 || bps!=16 || glImage->width!=width || glImage->height!=height ){
            status = -1;
            printf( "ERROR: read_16bitTIFF_glImage(): Can't read '%s' because input parameters are inconsistent.\n", TIFFname );
            printf( "/tActual parameters  : spp=%d, bps=%d, width=%d, height%d\n", spp, bps, width, height );
            printf( "/tExpected parameters: spp=%d, bps=%d, width=%d, heigth%d\n", 1, 16, glImage->width, glImage->height );
        }
    }
    
    if ( status == 0 ){
        lineBuffer = malloc( (long)(glImage->width)*sizeof(uint16) ); 
        if ( lineBuffer == NULL ) {
            status = -1;
            printf( "ERROR: read_16bitTIFF_glImage(): Failed to allocate memory for lineBuffer \n" );   
        } else {
            for ( j=0; j<glImage->height; j++ ){
                if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ){
                    printf( "ERROR: read_16bitTIFF_glImage(): Failed to read scanline %d from %s.\n", j, TIFFname );
                    status = -1;
                } else {
                    for( i=0; i<glImage->width; i++ ){
                        glImage->img[j][i] = (int)lineBuffer[i]; 
                    }
                }
            }
            free( lineBuffer );
        }
    }
    
    if ( inTIFF != NULL ){
        TIFFClose( inTIFF );
    }


    return status;
}


int read_3x8bitTIFF_rgbImage( char *fromTIFFname, rgbImage_t *rgbImage ) {
  int status = 0;
  
  int i, j;
  uint8 *lineBuffer; // for this variable changes were done and accordingly below
  int width, height;
  short spp, bps;
  TIFF *inTIFF;
  inTIFF = NULL;
  
  if ( rgbImage->img==NULL || rgbImage->memState!=1 ){
    status = -1;
    printf( "ERROR: read_3x8bitTIFF_grgbImage(): Invalid argument 'rgbImage->img==NULL || rgbImage->memState!=1'.\n" );
  }
  if ( status == 0 ){
    if ( (inTIFF = TIFFOpen( fromTIFFname, "r" )) == NULL ){
      status = -1;
      printf( "ERROR: failed to open TIFF.\n" );
    }
  }
  
  if ( status == 0 ){
    TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, &spp );
    TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
    TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, &width );
    TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, &height );
    if( spp!=3 || bps!=8 || rgbImage->width!=width || rgbImage->height!=height ){
      status = -1;
      printf( "ERROR: read_3x8bitTIFF_rgbImage(): Can't read '%s' because input parameters are inconsistent.\n", fromTIFFname );
      printf( "/tActual parameters  : spp=%d, bps=%d, width=%d, height%d\n", spp, bps, width, height );
      printf( "/tExpected parameters: spp=%d, bps=%d, width=%d, heigth%d\n", 3, 8, rgbImage->width, rgbImage->height );
    }
  }
  
  if ( status == 0 ){
    lineBuffer = malloc( (long)(rgbImage->width)*3*sizeof(uint8) ); 
    if ( lineBuffer == NULL ) {
      status = -1;
      printf( "ERROR: read_3x8bitTIFF_rgbImage(): Failed to allocate memory for lineBuffer \n" );   
    } else {
      for ( j=0; j<rgbImage->height; j++ ){
        if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ){
          printf( "ERROR: read_3x8bitTIFF_rgbImage(): Failed to read scanline %d from %s.\n", j, fromTIFFname );
          status = -1;
        } else {
          for( i=0; i<rgbImage->width; i++ ){
            rgbImage->img[0][j][i] = (int)lineBuffer[3*i]; 
            rgbImage->img[1][j][i] = (int)lineBuffer[3*i+1]; 
            rgbImage->img[2][j][i] = (int)lineBuffer[3*i+2]; 
          }
        }
      }
      free( lineBuffer );
    }
  }
  
  if ( inTIFF != NULL ){
    TIFFClose( inTIFF );
  }
  
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
  
  if ( rgbImage->img==NULL || rgbImage->memState!=1 ){
    status = -1;
    printf( "ERROR: read_3x16bitTIFF_grgbImage(): Invalid argument 'rgbImage->img==NULL || rgbImage->memState!=1'.\n" );
  }
  if ( status == 0 ){
    if ( (inTIFF = TIFFOpen( fromTIFFname, "r" )) == NULL ){
      status = -1;
      printf( "ERROR: failed to open TIFF.\n" );
    }
  }
  
  if ( status == 0 ){
    TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, &spp );
    TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
    TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, &width );
    TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, &height );
    if( spp!=3 || bps!=16 || rgbImage->width!=width || rgbImage->height!=height ){
      status = -1;
      printf( "ERROR: read_3x16bitTIFF_rgbImage(): Can't read '%s' because input parameters are inconsistent.\n", fromTIFFname );
      printf( "/tActual parameters  : spp=%d, bps=%d, width=%d, height%d\n", spp, bps, width, height );
      printf( "/tExpected parameters: spp=%d, bps=%d, width=%d, heigth%d\n", 3, 16, rgbImage->width, rgbImage->height );
    }
  }
  
  if ( status == 0 ){
    lineBuffer = malloc( (long)(rgbImage->width)*3*sizeof(uint16) ); 
    if ( lineBuffer == NULL ) {
      status = -1;
      printf( "ERROR: read_3x16bitTIFF_rgbImage(): Failed to allocate memory for lineBuffer \n" );   
    } else {
      for ( j=0; j<rgbImage->height; j++ ){
        if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ){
          printf( "ERROR: read_3x16bitTIFF_rgbImage(): Failed to read scanline %d from %s.\n", j, fromTIFFname );
          status = -1;
        } else {
          for( i=0; i<rgbImage->width; i++ ){
            rgbImage->img[0][j][i] = (int)lineBuffer[3*i]; 
            rgbImage->img[1][j][i] = (int)lineBuffer[3*i+1]; 
            rgbImage->img[2][j][i] = (int)lineBuffer[3*i+2]; 
          }
        }
      }
      free( lineBuffer );
    }
  }
  
  if ( inTIFF != NULL ){
    TIFFClose( inTIFF );
  }
  
  return status;
}

int write_16bitTIFF_glImage( glImage_t *glImage, char *toTIFFname ){
    int status = 0;
    
    TIFF *toTIFF;
    int j,i;
    uint16 *lineBuffer;
    int minGl, maxGl;
    
    if ( glImage->img==NULL || glImage->memState!=1 ){
        status = -1;
        printf( "ERROR: write_16bitTIFF_glImage(): Invalid argument 'glImage->img==NULL || glImage->memState!=1'.\n" ); 
    } else {
        status = minMax_glImage( glImage, &minGl, &maxGl );
        if ( status!=0 ) {
            printf( "ERROR: write_16bitTIFF_glImage(): Cannot determine gray level range.\n" );
        } else if ( minGl<0 || maxGl>65535 ) {
            status = -1;
            printf( "ERROR: write_16bitTIFF_glImage(): gl out of range: minGl=%d || maxGl=%d\n", minGl, maxGl );
        }
    }
    
    
    if ( status == 0 ) {
        if ( (toTIFF = TIFFOpen(toTIFFname, "w"))==NULL ){
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
            printf( "ERROR: write_16bitTIFF_glImage(): Failed to allocate memory for lineBuffer \n" );   
        } else {
            for ( j=0; j<glImage->height; j++ ){
                for ( i=0; i<glImage->width && status==0; i++ ){
                    lineBuffer[i] = (uint16) glImage->img[j][i];
                }
                if ( TIFFWriteScanline( toTIFF, lineBuffer, j, 0 ) == -1 ) {
                    status = -1;
                    printf ( "ERROR: write_16bitTIFF_glImage(): Failed to write scan line (%d)! status = %d\n", j, status );
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

  int c,j,i;
  int min[3], max[3];
  int width, height;
  TIFF *outTIFF;
  unsigned char *lineBuffer;    

  if ( rgbImage->img==NULL || rgbImage->memState!=1 ) {
    status = -1;
    printf ( "ERROR: write_3x8bitTIFF_rgbImage(): Inavlid argument 'rgbImage->img==NULL || rgbImage->memState!=1'.\n" );
  }
  
  // check if range of gray levels fits for storing as 8-bit image
  if ( status == 0 ) {
    status = get_range_rgbImage( rgbImage, min, max );
  }
  for ( c=0; c<2 && status==0; c++ ) {
    if ( min[c]<0 || max[c]>255 ) {
      status = -1;
      printf ( "ERROR: write_3x8bitTIFF_rgbImage(): Values out of range for 8bit rgb: red %d to %d, green %d to %d, blue %d to %d\n", min[0], max[0], min[1], max[1], min[2], max[2] );
    }
  } 

  if ( (outTIFF = TIFFOpen(toTIFFname, "w"))==NULL ){
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
      printf ( "ERROR : write_3x8bitTIFF_rgbImage(): failed to allocate memory for line buffer.\n" );
    }

    for ( j=0; j<height && status==0; j++ ){
      for ( i=0; i<width; i++ ){
	for ( c=0; c<3; c++ ){
	  lineBuffer[3*i+c] = (unsigned char)(rgbImage->img[c][j][i]);
	}
      }
      if (TIFFWriteScanline (outTIFF, lineBuffer, j, 0) == -1) {
	status = -1;
	printf ( "ERROR : write_3x8bitTIFF_rgbImage(): failed to write scan line (%d).\n", j );
      }   
    }
    free( lineBuffer );
    TIFFClose( outTIFF );
  }
  
  return status;
}

int writeROI_3x8bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFname, int roiXpos, int roiYpos, int roiWidth, int roiHeight ) {
  int status = 0;

  int c,j,i;
  int min[3], max[3];
  int width, height;
  TIFF *outTIFF;
  unsigned char *lineBuffer;    

  if ( rgbImage->img==NULL || rgbImage->memState!=1 ) {
    status = -1;
    printf ( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Inavlid argument 'rgbImage->img==NULL || rgbImage->memState!=1'.\n" );
  }

  if ( roiXpos+roiWidth>rgbImage->width || roiYpos+roiHeight>rgbImage->height ) {
    status = -1;
    printf ( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Inavlid arguments 'roiXpos+roiWidth>rgbImage.width || roiYpos+roiheight>rgbImage.height'\n" );
  }
  
  // check if range of gray levels fits for storing as 8-bit image
  if ( status == 0 ) {
    status = get_range_rgbImage( rgbImage, min, max );
  }
  for ( c=0; c<2 && status==0; c++ ) {
    if ( min[c]<0 || max[c]>255 ) {
      status = -1;
      printf ( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Values out of range for 8bit rgb: red %d to %d, green %d to %d, blue %d to %d\n", min[0], max[0], min[1], max[1], min[2], max[2] );
    }
  } 

  if ( (outTIFF = TIFFOpen(toTIFFname, "w"))==NULL ){
    status = -1;
    printf( "ERROR: writeROI_3x8bitTIFF_rgbImage(): Failed to open '%s' for writing.\n", toTIFFname );
  }

  if ( status == 0 ) {
    width = roiWidth;//rgbImage->width;
    height = roiHeight;//rgbImage->height;
    TIFFSetField( outTIFF, TIFFTAG_IMAGEWIDTH, width );
    TIFFSetField( outTIFF, TIFFTAG_IMAGELENGTH, height );
    TIFFSetField( outTIFF, TIFFTAG_BITSPERSAMPLE, 8 );
    TIFFSetField( outTIFF, TIFFTAG_SAMPLESPERPIXEL, 3 );
    TIFFSetField( outTIFF, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField( outTIFF, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    if ( (lineBuffer = malloc( 3*width*sizeof(unsigned char) )) == NULL ) {
      status = -1;
      printf ( "ERROR : writeROI_3x8bitTIFF_rgbImage(): failed to allocate memory for line buffer.\n" );
    }

    for ( j=0; j<roiHeight && status==0; j++ ){
      for ( i=0; i<roiWidth; i++ ){
	for ( c=0; c<3; c++ ){
	  lineBuffer[3*i+c] = (unsigned char)(rgbImage->img[c][j+roiYpos][i+roiXpos]);
	}
      }
      if (TIFFWriteScanline (outTIFF, lineBuffer, j, 0) == -1) {
	status = -1;
	printf ( "ERROR : writeROI_3x8bitTIFF_rgbImage(): failed to write scan line (%d).\n", j );
      }   
    }
    free( lineBuffer );
    TIFFClose( outTIFF );
  }
  
  return status;
}

int write_3x16bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFname ) {
  int status = 0;
  
  int c,j,i;
  int min[3], max[3];
  int width, height;
  TIFF *outTIFF;
  uint16 *lineBuffer;
  
  if ( rgbImage->img==NULL || rgbImage->memState!=1 ) {
    status = -1;
    printf ( "ERROR: write_3x16bitTIFF_rgbImage(): Inavlid argument 'rgbImage->img==NULL || rgbImage->memState!=1'.\n" );
  }
  
  // check if range of gray levels fits for storing as 8-bit image
  if ( status == 0 ) {
    status = get_range_rgbImage( rgbImage, min, max );
  }
  for ( c=0; c<2 && status==0; c++ ) {
    if ( min[c]<0 || max[c]>65535 ) {
      status = -1;
      printf ( "ERROR: write_3x16bitTIFF_rgbImage(): Values out of range for 16bit rgb: red %d to %d, green %d to %d, blue %d to %d\n", min[0], max[0], min[1], max[1], min[2], max[2] );
    }
  } 
  
  if ( (outTIFF = TIFFOpen(toTIFFname, "w"))==NULL ){
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
    
    if ( (lineBuffer = malloc( (long)(rgbImage->width)*3*sizeof(uint16) )) == NULL ) {
      status = -1;
      printf ( "ERROR : write_3x16bitTIFF_rgbImage(): failed to allocate memory for line buffer.\n" );
    }
    
    for ( j=0; j<height && status==0; j++ ){
      for ( i=0; i<width; i++ ){
        for ( c=0; c<3; c++ ){
          lineBuffer[3*i+c] = (uint16)(rgbImage->img[c][j][i]);
        }
      }
      if (TIFFWriteScanline (outTIFF, lineBuffer, j, 0) == -1) {
        status = -1;
        printf ( "ERROR : write_3x8bitTIFF_rgbImage(): failed to write scan line (%d).\n", j );
      }   
    }
    free( lineBuffer );
    TIFFClose( outTIFF );
  }
  
  return status;
}


// ******************************************
// *** basic information on image content ***
// ******************************************

int minMax_glImage( glImage_t *glImage, int *minGl, int *maxGl ){
    int status = 0;
    int j, i;
    
    if ( glImage->img == NULL ){
        printf( "ERROR: minMax_glImage(): Can't determine mix/max for an empty gl image.\n" );
        status = -1;
    }
    
    *minGl = glImage->img[0][0];
    *maxGl = *minGl;
    for ( j=0; j<glImage->height; j++ ) {
        for ( i=0; i<glImage->width; i++ ) {
            if ( glImage->img[j][i] > *maxGl ) {
                *maxGl = glImage->img[j][i];
            }
            if ( glImage->img[j][i] < *minGl ) {
                *minGl = glImage->img[j][i];
            }
        }
    }
    
    return status;
}


int get_range_rgbImage( rgbImage_t *rgbImage, int minValue[3], int maxValue[3] ) {
  int status = 0;

  int c;
  int i, j;
  int min, max;
  int width, height;

  if ( rgbImage->img==NULL || rgbImage->memState!=1 ) {
    status = -1;
    printf ( "ERROR: get_range_rgbImage(): Inavlid argument 'rgbImage->img==NULL || rgbImage->memState!=1'.\n" );
  } 

  if ( status == 0 ) {
    width = rgbImage->width;
    height = rgbImage->height;
    for ( c=0; c<3; c++ ) {
      min = max = rgbImage->img[c][0][0];
      for ( j=0; j<height; j++ ) {
	for ( i=0; i<width; i++ ) {
	  if ( rgbImage->img[c][j][i] < min ) {
	    min = rgbImage->img[c][j][i];
	  }
	  if ( rgbImage->img[c][j][i] > max ) {
	    max = rgbImage->img[c][j][i];
	  }
	}
      }
      minValue[c] = min;
      maxValue[c] = max;
    }
  }

  return status;
  }

int new_glHistogram( glHistogram_t *glHist, int minGl, int maxGl ) {
    int status = 0;
    int i;
     
    if ( glHist->freq!=NULL || glHist->memState!=0 ) {
        status = -1;
        printf( "ERROR: new_glHistogram(): Invalid arguments 'glHist->freq!=NULL || glHist->memState!=0'.\n" );
    } else if ( maxGl-minGl <= 0 ) {
        status = -1;
        printf( "ERROR: new_glHistogram(): Invalid gray level dynamic range: maxGl-minGl = %d - %d <= 0\n", maxGl, minGl );
        
    }
    
    if ( status == 0 ) {
        glHist->minGl = minGl;
        glHist->maxGl = maxGl;
        glHist->freq = malloc ( (maxGl+1)*sizeof(unsigned long) );
        if ( glHist->freq == NULL ) {
            status = -1;
            printf( "ERROR: new_glHistogram(): Failed to allocate memory for 'glHist->freq'\n" );
        }
    }
    
    if ( status == 0 ) {
        glHist->memState = 1;
    } else {
        glHist->memState = -1;
    }
    
    return status;
}

int delete_glHistogram( glHistogram_t *glHist ){
  int status = 0;

  if ( glHist->freq==NULL || glHist->memState==0 ) {
    status = -1;
    printf( "ERROR: delete_glHistogram(): nothing to do 'glHist->freq==NULL || glHist->memState==0'.\n" );
  }

  if ( status == 0 ) {
    free( glHist->freq );
    glHist->freq = NULL;
    glHist->memState = 0;
  }

  return status;
}

int get_glHistogram( glImage_t *glImage, glHistogram_t *hist ){
    int status = 0;
    int j, i;
    
    if ( glImage->img==NULL ){
        status = -1;
        printf( "ERROR: get_glHistogram(): Invalid gray level image: 'glImage->img == NULL'.\n " );
    } else if ( hist->freq==NULL ){
        status = -1;
        printf( "ERROR: get_glHistogram(): Invalid gray level histogram: 'hist->freq == NULL'\n " );
    }
    
    if ( status == 0 ) {
        for ( i=0; i<hist->maxGl+1; i++ ) {
            hist->freq[i] = 0;
        }
        for ( j=0; j<glImage->height; j++ ){
            for ( i=0; i<glImage->width; i++ ){
                hist->freq[glImage->img[j][i]]++;
            }
        }
    }
    
    return status;
}



int get_glHistograms_fourShot( glImage_t *glImage, glHistogram_t glHists[4] ) {
  int status = 0;

  int s;
  int i, j;

  if ( glImage->img==NULL ){
    status = -1;
    printf( "ERROR: get_glHistograms_fourShot(): Invalid gray level image: 'glImage->img == NULL'.\n " );
  } 
  else {
    for ( s=0; s<4 && status==0; s++ ) {
      if ( glHists[s].freq==NULL || glHists[s].memState!=1 ){
	status = -1;
	printf( "ERROR: get_glHistograms_fourShot(): Invalid gray level histogram: 'glHists[%d].freq==NULL || glHists[i].memState!=1'\n", i );
      }
    }
  }

  if ( status == 0 ) {
    for ( s=0; s<4; s++ ) {
      for ( i=0; i<glHists[s].maxGl+1; i++ ) {
	glHists[s].freq[i] = 0;
      }
    }
    for ( j=0; j<glImage->height-1; j+=2 ) {    
      for ( i=0; i<glImage->width-1; i+=2 ) {
	glHists[0].freq[glImage->img[j][i]]++;
	glHists[1].freq[glImage->img[j][i+1]]++;
	glHists[2].freq[glImage->img[j+1][i]]++;
	glHists[3].freq[glImage->img[j+1][i+1]]++;
      }
    }
  }

  return status;
}


int write_glHistogram( glHistogram_t *hist, char *fileName ){
    int status = 0;
    
    FILE *histFile;
    int gl;
    
    if ( hist->freq==NULL ) {
        status = -1;
        printf( "ERROR: write_glHistogram(): Can't write empty histogram to disk.\n" );
    } else if ( (hist->minGl==hist->maxGl) && (hist->freq[hist->maxGl]==0) ) {
        status = -1;
        printf( "ERROR: write_glHistogram(): Can't write empty histogram to disk.\n" );
    }
    
    if ( status == 0 ){
        if ( (histFile = fopen( fileName, "w" )) == NULL ) {
            status = -1;
            printf( "ERROR: write_glHistogram(): Cannot open file %s for writing.\n", fileName );
        }
        if ( status == 0 ) {
            fprintf( histFile, "gl\tfreq\n" );
            for ( gl = hist->minGl; gl<hist->maxGl; gl++ ) {
                fprintf( histFile, "%d\t%lu\n", gl, hist->freq[gl] );
            }
            fclose( histFile );
        }
    }
    
    return status;
}



int write_glHistograms_fourShot( glHistogram_t glHists[4], char *toFileName ) {
  int status = 0;
    
  FILE *histsFile;
  int gl;
  int minGl, maxGl;
  int s;
  
  for ( s=0; s<4 && status==0; s++ ) {
    if ( glHists[s].minGl!=glHists[0].minGl || glHists[s].maxGl!=glHists[0].maxGl ) {
      status = -1;
      printf( "ERROR: write_glHistograms_fourShot(): Unequal ranges of the histogams: 'glHists[%d].minGl!=glHists[0].minGl || glHists[%d].maxGl!=glHists[0].maxGl'\n", s, s );
    }
    else if ( glHists[s].freq==NULL || glHists[s].memState!=1 ){
      status = -1;
      printf( "ERROR: write_glHistograms_fourShot(): Invalid gray level histogram: 'glHists[%d].freq==NULL || glHists[%d].memState!=1'\n", s, s );
    }
    else if ( (glHists[s].minGl==glHists[s].maxGl) && (glHists[s].freq[glHists[s].maxGl]==0) ) {
      status = -1;
      printf( "ERROR: write_glHistograms_fourShot(): Can't write empty histogram to disk (shot %d).\n", s );
    }
  }
  
  if ( status == 0 ){
    minGl = glHists[0].minGl;
    maxGl = glHists[0].maxGl;
    if ( (histsFile = fopen( toFileName, "w" )) == NULL ) {
      status = -1;
      printf( "ERROR: write_glHistograms_fourShot(): Cannot open file %s for writing.\n", toFileName );
    }
    if ( status == 0 ) {
      fprintf( histsFile, "gl\tfreq(s=0)\freq(s=1)\tfreq(s=2)\tfreq(s=3) where 's' ist the shot\n" );
      for ( gl = minGl; gl<maxGl; gl++ ) {
	fprintf( histsFile, "%d\t%lu\t%lu\t%lu\t%lu\n", gl, glHists[0].freq[gl], glHists[1].freq[gl], glHists[2].freq[gl], glHists[3].freq[gl] );
      }
      fclose( histsFile );
    }
  }
  
  return status;
}


int new_glProfile( glProfile_t *glProfile, int length ) {
  int status = 0;
  
  if ( glProfile->glSum!=NULL || glProfile->memState!=0 ) {
    status = -1;
    printf( "ERROR: new_glProfile(): gray level profile is not empty 'glProfile->glSum!=NULL || glProfile->memState!=0'\n" );
  } else if ( length < 0 ) {
    status = -1;
    printf( "ERROR: new_glProfile(): invalid argument 'length<0'\n" );
  }

  if ( status == 0 ) {
    //printf( "prior having assigned a value: glProfile->p.x = %d\n", glProfile->p.x );
    glProfile->p.x = -1;
    //printf( "after assingning -1 to it: glProfile->p.x = %d\n", glProfile->p.x );
    glProfile->p.y = -1;
    glProfile->direction = -1;
    glProfile->length = length;
    glProfile->thickness = -1;
    if ( (glProfile->glSum = malloc( length*sizeof(long) )) == NULL ) {
      status = -1;
      glProfile->memState = -1;
      printf ( "ERROR: new_glProfile(): Failed to allocat ememory for 'glProfile->glSum'\n" );
    } else {
      glProfile->memState = 1;
    }
  }

  return status;
}

int delete_glProfile( glProfile_t *glProfile ){
  int status = 0;

  if ( glProfile->glSum==NULL || glProfile->memState==0 ) {
    status = -1;
    printf( "ERROR: delete_glProfile(): Cannot free memory 'glProfile->glSum==NULL || glProfile->memState==0'\n" );
  }
  if ( status == 0 ) {
    free( glProfile->glSum );
    glProfile->glSum = NULL;
    glProfile->memState = 0;
  }
  return status;
}

int get_glProfile( glImage_t *glImage, glProfile_t *glProfile, point_t p, int direction, int thickness ) {
  int status = 0;

  int sum;
  int i, j;

  if ( glImage->img == NULL ) {
    status = -1;
    printf( "ERROR: get_glProfile_ROI(): Image is not allocated 'glImage->img==NULL'!\n" );
  } 
  else if ( glProfile->glSum==NULL || glProfile->memState!=1 ) {
    status = -1;
    printf( "ERROR: get_glProfile_ROI(): Profile is not allocated 'glProfile->glSum==NULL' or in unknown state 'glProfile->memState!=1'!\n" );
  } 
  else if ( p.x<0 || p.x>glImage->width-1 || p.y<0 || p.y>glImage->height-1 ) {
    status = -1;
    printf( "ERROR: get_glProfile_ROI(): Point p=(%d,%d) outside image!\n", p.x, p.y );
  }
  else {
    if ( direction == 0 ) {
      if ( (p.x+glProfile->length > glImage->width) || (p.y+thickness > glImage->height) ) {
	status = -1;
	printf( "ERROR: get_glProfile_ROI(): ROI (%d,%d) to (%d,%d) outside image!\n", p.x, p.y, p.x+glProfile->length, p.y+thickness );
      }
    } 
    else if ( direction == 1 ) {
      status = -1;
      printf( "NOT YET IMPLEMETED, SORRY!\n" );
    }
    else {
      status = -1;
      printf( "ERROR: get_glProfile_ROI(): invalid value 'direction=%d'\n", direction );
    }
  }

  if ( status == 0 ) {
    glProfile->p.x = p.x;
    glProfile->p.y = p.y;
    glProfile->thickness = thickness;
    glProfile->direction = direction;
    if ( direction == 0 ) {
      for ( i=0; i<glProfile->length; i++ ) {
	glProfile->glSum[i] = 0;
	for ( j=0; j<thickness; j++ ) {
	  glProfile->glSum[i] += glImage->img[p.y+j][p.x+i];
	}
      }
    }
    if ( direction == 1 ) {
      // ...
    }
  }

  return status;
}


int write_glProfile( glProfile_t *glProfile, char *fileName ) {
  int status = 0;
  
  FILE *file;
  int i,j;
  int x, y;
  
  if ( glProfile->glSum==NULL || glProfile->memState!=1 ) {
    status = -1;
    printf( "ERROR: write_glProfile(): gray level profile is empty 'glProfile->glSum==NULL || glProfile->memState!=1'\n" );
  } 
  else if ( glProfile->length < 0 ) {
    status = -1;
    printf( "ERROR: write_glProfile(): 'glProfile->length<0'\n" );
  } 
  
  if ( status == 0 ) {
    if ( (file=fopen( fileName, "w" )) == NULL ) {
      status = -1;
      printf( "ERROR: write_glProfile(): Failed to open '%s' for writing.\n", fileName );
    }
    if ( status == 0 ) {
      fprintf( file, "direction: %d \t\t(0 <-> along x-axis, 1 <-> along y-axis\n", glProfile->direction );
      fprintf( file, "point: (%d,%d) \t(upper left point of used rectangle)\n", glProfile->p.x, glProfile->p.y );
      fprintf( file, "length: %d \t\t(along 'direction')\n", glProfile->length);
      fprintf( file, "thickness: %d \t\t(number of gray levels summed, i.e. dimension perpendicular to 'direction')\n", glProfile->thickness );
      if ( glProfile->direction == 0 ) {
	fprintf( file, "x\tglSum\n" );
	x = glProfile->p.x;
	for ( i=0; i<glProfile->length; i++ ) {
	  fprintf( file, "%d\t%ld\n", x+i, glProfile->glSum[i] );
	}
      } 
      if ( glProfile->direction == 1 ) {
	fprintf( file, "y\tglSum\n" );
	y = glProfile->p.y;
	for ( j=0; j<glProfile->length; j++ ) {
	  fprintf( file, "%d\t%ld\n", y+j, glProfile->glSum[j] );
	}
      }
      fclose( file );
    }
  }
  
  return status;
}



int copy_glProfile( glProfile_t *glProfileORI, glProfile_t *glProfileCPY ) {
    int status = 0;
    
    int i;
    
    if ( glProfileORI->glSum==NULL || glProfileORI->memState!=1 ) {
        status = -1;
        printf ( "ERROR: copy_glProfile(): Invalid argument 'glProfileORI->glSum==NULL || glProfileORI->memState!=1'\n" );
    } 
    else if ( glProfileCPY->glSum==NULL || glProfileCPY->memState!=1 ) {
        status = -1;
        printf ( "ERROR: copy_glProfile(): Invalid argument 'glProfileCPY->glSum==NULL || glProfileCPY->memState!=1'\n" );
    } 
    else if ( glProfileORI->length < 0 ) {
        status = -1;
        printf ( "ERROR: copy_glProfile(): Invalid argument 'glProfileORI->length<0'\n" );
    } 
    else if ( glProfileCPY->length < 0 ) {
        status = -1;
        printf ( "ERROR: copy_glProfile(): Invalid argument 'glProfileCPY->length<0'\n" );
    } 
    else if ( glProfileORI->length != glProfileCPY->length ) {
        status = -1;
        printf ( "ERROR: copy_glProfile(): Inconsitent length of profiles 'glProfileORI->length != glProfileCPY->length'.\n" ); 
    }
    
    if ( status == 0 ) {
        glProfileCPY->p.x = glProfileORI->p.x;
        glProfileCPY->p.y = glProfileORI->p.y;
        glProfileCPY->thickness = glProfileORI->thickness;
        for ( i=0; i<glProfileORI->length; i++ ) {
            glProfileCPY->glSum[i] = glProfileORI->glSum[i];
        }
    }
    
    return status;
}


int smooth_glProfile( glProfile_t *glProfile ) {
    int status = 0;
    
    int i, ii;
    long buf;
    int kL = 7;
    int kernel[7] = {1, 2, 4, 5, 4, 2, 1};
  //int kernel[9] = {1, 2, 3, 5, 6, 5, 3, 2, 1};
    int kC = 3;
    int Z = 0;
    for ( i=0; i<kL; i++ ) {
        Z+=kernel[i];
    }
    glProfile_t profileBuf;
    profileBuf.glSum = NULL;
    profileBuf.memState = 0;
    
    if ( glProfile->glSum==NULL || glProfile->memState!=1 ) {
        status = -1;
        printf ( "ERROR: smooth_glProfile(): Invalid argument 'glProfile->glSum==NULL || glProfile->memeState!=1'\n" );
    } else if ( glProfile->length < 5 ) {
        status = -1;
        printf ( "ERROR: smooth_glProfile(): Invalid argument 'glProfile->length<5'\n" );
    }
    
    if ( status == 0 ) {
        status = new_glProfile( &profileBuf, glProfile->length );
        if ( status == 0 ) {
            status = copy_glProfile( glProfile, &profileBuf );
        }
        //glProfile->glSum[0] = (int)((glProfile->glSum[0]+kernelglProfile->glSum[1]+glProfile->glSum[2]
        if ( status == 0 ) {
            for ( i=0; i<glProfile->length-kL; i++ ) {
                buf = 0;
                for ( ii=0; ii<kL; ii++ ) {
                    buf += kernel[ii]*profileBuf.glSum[i+ii];
                }
                glProfile->glSum[i+kC] = (long)( (float)buf/Z + 0.5);
            }    
        }
        
        if ( profileBuf.memState != 0 ) {
            delete_glProfile( &profileBuf );
        }
        
    }
    
    return status;
}

/*
int erode_glProfile( glProfile_t *glProfile ) {
  int status = 0;

  int i;

  if ( glProfile->glSum==NULL || glProfile->memState!=1 ) {
    status = -1;
    printf ( "ERROR: erode_glProfile(): Invalid argument 'glProfile->glSum==NULL || glProfile->memeState!=1'\n" );
  } else if ( glProfile->length < 0 ) {
    status = -1;
    printf ( "ERROR: erode_glProfile(): Invalid argument 'glProfile->length<0'\n" );
  }

  if ( status == 0 ) {
    for ( i=0; i<glProfile->length; i++ ) {

    }
  }  

  return status;
}
*/


int extract_localMinima_glProfile( glProfile_t *profile ) {
  int status = 0;

  int min;
  glProfile_t buf;
  buf.glSum = NULL;
  buf.memState = 0;
  int i;

  if ( profile->glSum==NULL || profile->memState!=1 ) {
    status = -1;
    printf( "ERROR: get_localMinima_glProfile(): Invalid argument 'glProfile->list==NULL || glprofile->memState!=1'\n" );
  }

  if ( status == 0 ) {
    //printf( "Hudigak! buf.memState=%d (buf.glSum==NULL)=%d (1==0)=%d (1==1)=%d\n", buf.memState, (buf.glSum==NULL), (1==0), (1==1) );
    status = new_glProfile( &buf, profile->length );
    /*if ( status == 0 ) {
      status = copy_glProfile( &profile2, profile );
    }
    */
    // 2. compute the 1D erosion and store it in the buffer, boundaries are set to 0 by default
    if ( status == 0 ) {
      buf.glSum[0] = buf.glSum[profile->length-1] = 0;
      //status = erode_glprofile( &profile2 );
      for ( i=1; i<profile->length-1; i++ ) {
        min = profile->glSum[i];
        if ( profile->glSum[i-1] < min ) {
          min = profile->glSum[i-1];
        }
        if ( profile->glSum[i+1] < min ) {
          min = profile->glSum[i+1];
        }
        buf.glSum[i] = min; 
      }
    }
    // 3. compare original and eroded profile -> keep only local minima
    if ( status == 0 ) {
      for ( i=0; i<profile->length; i++ ) {
        if ( profile->glSum[i] > buf.glSum[i] ) {
          profile->glSum[i] = 0;
        }
      }
    }
    // 4. delete the eroded profile
    status = delete_glProfile( &buf );
  }

  return status;
} 



int extract_wells_glProfile( glProfile_t *profile ) {
  int status = 0;

  glProfile_t profileCpy;
  profileCpy.glSum = NULL;
  profileCpy.memState = 0;
  int i, ii;
  int buf;  
  int mask[7] = { -5, 0, 3, 4, 3, 0, -5 };
  int maskCenter = 3;
  int maskLen = 7;

  long locMeanSignal;
  int inspectedSignals;

  float meanSignal;
  int nSignals;
  long minSignal, maxSignal;

  int signalHist[40];
  float binSize;

  if ( profile->glSum==NULL || profile->memState!=1 ) {
    status = -1;
    printf( "ERROR: extract_wells_glprofile(): Invalid argument 'glProfile->list==NULL || glprofile->memState!=1'\n" );
  }

  if ( status == 0 ) {
    // 1. produce a copy of the gl profile
    status = new_glProfile( &profileCpy, profile->length );
    if ( status == 0 ) {
      status = copy_glProfile( profile, &profileCpy );
    }
    // 2. compute the convolution with the mask
    if ( status == 0 ) {
      printf( ">> computing convolution with well mask..\n" );
      for ( i=0; i<profileCpy.length-maskLen; i++ ) {
	profile->glSum[i+maskCenter] = 0;
	for ( ii=0; ii<maskLen; ii++ ) {
	  profile->glSum[i+maskCenter] += mask[ii]*profileCpy.glSum[i+ii];
	}
      } 
      for ( i=0; i<maskCenter; i++ ) {
	profile->glSum[i] = profile->glSum[maskLen];
      }
      for ( i=profileCpy.length-maskLen; i<profileCpy.length; i++ ) {
	profile->glSum[i] = profile->glSum[profileCpy.length-maskLen-1];
      }
      printf( ">> OK.\n" );
    }
    
    // 3. extract the local minima  
    if ( status == 0 ) {
      printf( ">> extracting local minima\n" );
      status = extract_localMinima_glProfile( profile );
      if ( status == 0 ) {
	printf( ">> OK.\n" );
      }
    }
    
    // 4.a. throw out all positive signals
    for ( i=0; i<profile->length; i++ ) {
      if ( profile->glSum[i] > 0 ) {
	profile->glSum[i] = 0;
      }
    }

    // 4.b. throw out signal close to left and right edge
    for ( i=0; i<profile->length/20; i++ ) {
      profile->glSum[i] = profile->glSum[profile->length-1-i] = 0;
    }
    
    
    // 5. throw out all signals that are higher than a threshold calculated from the local mean of 9 signals
    printf( "i\tsignal\tinspSgnls locmeanSignal\n" ); 
    for ( i=0; i<profile->length; i++ ) {
      if ( profile->glSum[i] != 0 ) {
	printf( "%d\t%ld ", i, profile->glSum[i] );
	locMeanSignal = 0;
	inspectedSignals = 0;
	for ( ii=i+1; ii<profile->length && inspectedSignals<9; ii++ ) {
	  if ( profile->glSum[ii] != 0 ) {
	    locMeanSignal += profile->glSum[ii];
	    inspectedSignals++;
	  }
	}
	if ( inspectedSignals > 0 ) { 
	  locMeanSignal /= inspectedSignals;
	  printf( "%d %ld\t", inspectedSignals, locMeanSignal );
	  if ( profile->glSum[i] > (int)(locMeanSignal*0.8) ) { ///////////////// Here ist the parameter whose value in rgb_HR_new_??? is referred to as e.g. 0p8 
	    printf( "out!" );
	    profile->glSum[i] = 0;
	  }
	  printf( "\n" );
	}
      }
    }
    
    /*
    // get some simple statistics from signals
    meanSignal = 0;
    nSignals = 0;
    for ( i=0; i<profile->length && nSignals==0; i++ ) {
      if ( profile->glSum[i] != 0 ) {
	nSignals++;
	meanSignal+=profile->glSum[i];
	minSignal = maxSignal = profile->glSum[i];
      }
    }
    if ( nSignals == 0 ) {
      status = -1;
      printf( "ERROR: extract_wells_glProfile(): No signals from wells detected.\n" );
    }
    for ( i=i+1; i<profile->length; i++ ) {
      if ( profile->glSum[i] != 0 ) {
	nSignals++;
	meanSignal+=profile->glSum[i];
	if ( profile->glSum[i] < minSignal ) {
	  minSignal = profile->glSum[i];
	}
	if ( profile->glSum[i] > maxSignal ) {
	  maxSignal = profile->glSum[i];
	}
      }
    }
    meanSignal /= nSignals;
    printf( "nSignal = %d\n", nSignals );
    printf( "meanSignal = %f, minSignal = %ld, maxSignal = %ld\n", meanSignal, minSignal, maxSignal );
    // aim is to throw out signals larger than the mean signal if signal spread is large
    // more statistics ...
    for ( i=0; i<40; i++ ) {
      signalHist[i] = 0;
    }
    binSize = (maxSignal - minSignal)/41;
    for ( i=0; i<profile->length; i++ ) {
      if ( profile->glSum[i] != 0 ) {
	signalHist[(int)((profile->glSum[i]-minSignal)/binSize)]++;
      }
    }
    printf( "Signal histogram, binSize = %f\n", binSize );
    printf( "binNo\tfreq\n" );
    for ( i=0; i<40; i++ ) {
      printf( "%d\t%d\n", i, signalHist[i] );
    }
    */
    // 6. copy gl values back from copy to non zero entries of profile
    for ( i=0; i<profile->length; i++ ) {
      if ( profile->glSum[i] != 0 ) {
	profile->glSum[i] = profileCpy.glSum[i];
      }
    }
    
    // delete the copy
    if ( delete_glProfile( &profileCpy ) != 0 ) {
      status = -1;
      printf( "ERROR: extract_wells_glProfile(): Failed to free 'profileCpy'.\n" );
    }

  }

  return status;
}


int get_meanPeakSpacing( glProfile_t *profile, float *peakSpacing ){
    int status = 0;
    
    int i, prev_i;
    int threshold;
    int nSpaces;
    int sum;
    
    if ( profile->glSum==NULL || profile->memState!=1 ) {
        status = -1;
        printf( "ERROR: get_meanRasterSpacing(): Invalid argument 'profile->glSum==NULL || profile->memState!=1'\n" );
    }

    if ( status == 0 ) {
        threshold = 0;
        sum = 0;
        while ( prev_i==-1 && i<profile->length ) {
            if ( profile->glSum[i] > threshold ) {
                prev_i = i;
            }
            i++;
        }
        if ( i==profile->length ) {
            status = -1;
            printf( "ERROR: get_meanRasterSpacing(): No peaks found in profile!\n" );
        }
        if ( status == 0 ) {
            nSpaces = 0;
            for ( i=prev_i+1; i<profile->length; i++ ) {
                if ( profile->glSum[i] > threshold ) {
                    sum += i - prev_i;
                    prev_i = i;
                    nSpaces++;
                }
            }
            if ( nSpaces == 0 ) {
                status = -1;
                printf( "ERROR: get_meanRasterSpacing(): Only one peak at i=%d in profile.\n", prev_i );
            }
        }
        if ( status == 0 ) {
            *peakSpacing = (int)((float)sum/nSpaces);
        }
    }
    
    return status;
}



int get_peakSpacingHistogram( glProfile_t *glProfile, int peakSpacingHist[40], int *nPeaks ) {
    int status = 0;

    int threshold = 0;
    int i, prev_i;
    int dist;
    int n;
    
    if ( glProfile->glSum==NULL || glProfile->memState!=1 ) {
        status = -1;
        printf( "ERROR: get_peakSpacingHistogram(): Invalid argument 'glProfile->glSum==NULL || glProfile->memState!=1'\n" );
    }
    
    if ( status == 0 ) {
        for ( i=0; i<40; i++ ) {
            peakSpacingHist[i] = 0;            
        }
        //printf( "Looking for first peak\n" );
        prev_i = -1;
        i = 0;
        while ( prev_i==-1 && i<glProfile->length ) {
            if ( glProfile->glSum[i] > threshold ) {
                prev_i = i;
            }
            i++;
        }
        //printf( "first peak at %d\n", prev_i );
        if ( i == glProfile->length ) {
            status = -1;
            printf( "ERROR: get_PeakSpacingHistogram(): No peaks found in profile!\n" );
        }
        if ( status == 0 ) {
            dist = 0;
            n = 1;
            for ( i=prev_i+1; i<glProfile->length; i++ ) {
                if ( glProfile->glSum[i] > threshold ) {
                    //printf( "next peak at %d\t", i );
                    dist = i - prev_i;
                    //printf( "dist = %d\n", dist );
                    prev_i = i;
                    n++;
                    if ( dist < 40 ) {
                        peakSpacingHist[dist]++;
                    }
                    else {
		      //status = -1;
                        printf( "WARNING: get_PeakSpacingHistogram(): peak spacing too large 'dist > 40' --> ignoring this distance.\n" );
                    }
                }
            }
            if ( n > 1 ) {
                *nPeaks = n;
            }
            else { 
                status = -1;
                printf( "ERROR: get_PeakSpacingHistogram(): Only one peak found in profile!\n" );
            } 
            /*if ( status == 0 ) {
                printf( "dist\tcount\n" );
                for ( i=0; i<40; i++ ) {
                    if ( peakSpacingHist[i] > 0 ) {
		       printf( "%d\t%d\n", i, peakSpacingHist[i] ); 
                    }
		}
	    }*/
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
  }
  else {
    if ( *rasterSpacing > 3 ) {
      printf( "WARNING: Could not determine regular distance of raster, using value from previous frame.\n" );
    }
    else {
      status = -1;
      printf( "ERROR: Could not determine regular distance of raster./n" );
    }
  }

  return status;
}


int regularize_peakRaster( glProfile_t *glProfile, int rasterSpacing, int *nPeaks ) {
  int status = 0;
  
  int threshold = 0;
  
  int i_prev, i_next, i, ii;
  int i_start, i_end;
  int dist;
  float fixDist;
  int nPeaksToFix;
  float effectiveMeanSpacing; // to become an argument whose value is set
  int extractedRegDistances;
  int nAddedPeaks_left, nAddedPeaks_right, nAddedPeaks_middle;
  int nRemovedPeaks;
    
  int *regPeaks;
  int memState_regPeaks = 0;
  int nRegPeaks = 0;
  
  if ( glProfile->glSum==NULL || glProfile->memState!=1 ) {
    status = -1;
    printf( "ERROR: regularize_peakRaster(): Invalid argument 'glProfile->glSum==NULL || glProfile->memState!=1'\n" );
  } else if ( glProfile->direction != 0 ) {
    status = -1;
    printf( "ERROR: regularize_peakRaster(): Invalid argument 'glProfile->direction!=0'\n" );
  } else if ( rasterSpacing < 4 ) {
    status = -1;
    printf( "ERROR: Invalid argument 'rasterSpacing=%d < 4'/n", rasterSpacing  );
  }
  
  if ( status == 0 ) {
    i_prev = -1;
    i = 0;
    while ( i_prev ==-1 && i<glProfile->length ) {
      if ( glProfile->glSum[i] > threshold ) {
        i_prev = i;
      }
      i++;
    }
    if ( i_prev == -1 ) { 
      *nPeaks = 0;
      printf( "WARNING: regularize_peakRaster(): First peak not found in profile! Skipping regularization of raster!\n" );
    }
    else {
      *nPeaks = 1;
    }
  }
  if ( status==0  && *nPeaks>0 ) {
    if ( (regPeaks = malloc( glProfile->length*sizeof(int) )) == NULL ){
      status = -1;
      printf( "ERROR: regularize_peakRaster(): Failed to allocate memory for 'regPeaks'.\n" );                
    }
    else {
      memState_regPeaks = 1;
      for ( i=0; i<glProfile->length; i++ ) {
        regPeaks[i] = 0;
      }
    }
  }
  if ( status==0 && *nPeaks>0 ) {
    printf( "> looking for regular peaks, i.e. such spaced by rasterSpacing=%d, starting from first peak detected at %d\n", rasterSpacing, i_prev );
    extractedRegDistances = 0;
    i = i_prev + 1;
    while ( i<glProfile->length ) {
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
  
  if ( status==0 && *nPeaks>0 ) {
    i_prev = -1;
    dist = -1;
    nRemovedPeaks = 0;
    //printf( "extracted peaks:\ni\ttype\tdist to prev\n" ); 
    for ( i=0; i<glProfile->length; i++ ) {
      if ( regPeaks[i] == 1 ) {
        if ( i_prev == -1 ) {
          i_prev = i;
        }
        else {
          dist = i - i_prev;
          if ( dist!=rasterSpacing && (dist<(3*rasterSpacing)/2) ) {
            //printf( "> removing peaks at %d and %d spaced by dist=%d!=rasterSpacing && dist<(3*rasterSpacing)/2\n",i_prev, i, dist );
            regPeaks[i_prev] = regPeaks[i] = -1;
            nRemovedPeaks += 2;
          }
          i_prev = i;
        }
        //printf( "%d\t%d\t%d\n", i, regPeaks[i], dist );
        nRegPeaks++;
      }
    }
    printf( "removed %d of the extracted peaks because they formed an odd distance to neighboring peaks\n", nRemovedPeaks );
    nRegPeaks = 0;
    i_prev = -1;
    dist = -1;
    for ( i=0; i<glProfile->length; i++ ) {
      if ( regPeaks[i] == 1 ) {
        if ( i_prev == -1 ) {
          i_prev = i;
        }
        else {
          dist = i - i_prev;
          i_prev = i;
        }
        nRegPeaks++;
      }
    }
    printf( "The number of well spaced extracted peaks is %d .\n", nRegPeaks );
  }
  
  if ( status==0 && nRegPeaks>0 ) {
    printf( "> Filling in missing peaks to form a complete regular raster\n" );
    // left boundary region first
    for ( i=0; i<glProfile->length; i++ ) {
      if ( regPeaks[i] == 1 ) {
        i_prev = i;
        i = glProfile->length;
      }
    }
    printf( "leftmost extracted regular peak is at i=%d\n", i_prev );
    nAddedPeaks_left = 0;
    for ( i=i_prev-rasterSpacing; i>0; i-=rasterSpacing ) {
      regPeaks[i] = 2; 
      nRegPeaks++;
      nAddedPeaks_left++;
    }
    printf( "Added %d peaks at left boundary.\n", nAddedPeaks_left );
    // right boundary region
    for ( i=glProfile->length; i>i_prev; i-- ) {
      if ( regPeaks[i] == 1 ) {
        i_next = i;
        i = i_prev;
      }
    }
    printf( "rightmost extracted regular peak is at i=%d\n", i_next );
    nAddedPeaks_right = 0;
    for ( i=i_next+rasterSpacing; i<glProfile->length; i+=rasterSpacing ) {
      regPeaks[i] = 2;
      nRegPeaks++;
      nAddedPeaks_right++;
    }
    printf( "Added %d peaks at right boundary.\n", nAddedPeaks_right );
    // and the middle region
    i_start = i_prev;
    i_end = i_next;
    printf( "now fixing missing peaks in gaps located between %d and %d\n", i_start, i_end );
    nAddedPeaks_middle = 0;
    for ( i=i_start; i<i_end; i+=rasterSpacing ) {
      if ( regPeaks[i] < 1 ) {
        ii = i+1;
        while (  regPeaks[ii]<1 && ii<=i_end ) {
          ii++;
        }
        i_next = ii;
        dist = (i_next-i_prev);
        printf( "\tgap of %d pixels (from %d to %d)\t", dist, i_prev, i_next );
        nPeaksToFix = (int)(((float)dist)/rasterSpacing + 0.5) - 1;
        fixDist = (float)dist/(nPeaksToFix+1);
        printf( "--> npeaksToFix = %d, spaced by %f :\n\t\t", nPeaksToFix, fixDist );   
        for ( ii=1; ii<=nPeaksToFix; ii++ ) {
          printf( "|%f ", i_prev+ii*fixDist );
          regPeaks[i_prev+(int)(ii*fixDist + 0.5)] = 3;
          nAddedPeaks_middle++;
        }
        printf( "\n" );
        i_prev = i_prev+(int)(ii*fixDist + 0.5);
        i = i_next;
      } 
      else {
        //printf( "peak at %d present as expected\n", i );
        i_prev = i;
      }
    }
    printf( "Added %d peaks in the middle to fill gaps.\n", nAddedPeaks_middle );
  }
  
  if ( status==0 && nRegPeaks>0 ) {
    //printf( "regularized raster:\n   i\ttype (1 <-> detected, 2 <-> reconstructed, -1 <-> removed)\n" );
    // find distance in pixel from first to last peak and count how many peaks are there
    // update the profile to contain the regularized raster only
    nRegPeaks = 0;
    i_prev = i_next = -1;
    dist = -1;
    for ( i=0; i<glProfile->length; i++ ) {
      if ( regPeaks[i] > 0 ) {
        nRegPeaks++;
        if ( i_prev == -1 ) {
          i_prev = i;
          ii = i;
        }
        else {
          dist = i - ii;
          ii = i;
        }
        if ( i > i_next ) {
          i_next = i;
        }
        //printf( "% 4d\t%d -- dist to prev = %d\n", i, regPeaks[i], dist );
        glProfile->glSum[i] = regPeaks[i];
      }
      else {
        glProfile->glSum[i] = 0;
      }
    }
    dist = i_next -i_prev;
    effectiveMeanSpacing = (float)dist/(nRegPeaks-1);
    printf( "nRegPeaks = %d, total distance = %d, effective mean spacing = %f\n", nRegPeaks, dist, effectiveMeanSpacing );
    *nPeaks = nRegPeaks;
    // remove all but the regular peaks from the glProfile
    // add missing peaks to the glProfile
    // count the peaks and update nPeaks
    // check if nPeaks and the distance from forst to last match the rasterSpacing
  }
  
  if ( memState_regPeaks != 0 ) {
      free( regPeaks );
  }
    
  return status;
}


int relax_regPeakRaster( glProfile_t *glProfile, glProfile_t *regPeakProfile ) {
  int status = 0;

  int maxDist = 4; // more than 4 is rather seldom, so its not worthmaking this and input option
  printf( "> maxDist = %d\n", maxDist );
  int dist;
  int glBuf;
  int iBuf;
  int i;
  int shift;
  
  if ( glProfile->glSum==NULL || glProfile->memState!=1 ) {
    status = -1;
    printf( "ERROR: relax_regPeakRaster(): Invalid argument 'glProfile->glSum==NULL || glProfile->memState!=1'\n" );
  }
  if ( regPeakProfile->glSum==NULL || regPeakProfile->memState!=1 ) {
    status = -1;
    printf( "ERROR: relax_regPeakRaster(): Invalid argument 'regPeakProfile->glSum==NULL || regPeakProfile->memState!=1'\n" );
  }
  if ( glProfile->length != regPeakProfile->length ) {
    status = -1;
    printf( "ERROR: relax_regPeakRaster(): Inconsistent profile length ' glProfile->length != regPeakProfile->length'\n" );
  }

  if ( status == 0 ) {
    for ( i=maxDist; i<regPeakProfile->length-maxDist; i++ ) {
      if ( regPeakProfile->glSum[i] > 0 ) { // > 1 --> only affect those positions that were filled in, > 0 --> affect all positions
        // new ideas : A) cross correlation with a "hole" / B) look for absolute minimum within a small region
        // trying B) first
        glBuf =  glProfile->glSum[i];
        shift = 0;
        for ( dist=-maxDist; dist<=maxDist; dist++ ) {
          if ( glProfile->glSum[i+dist] < glBuf ) {
            shift = dist;
            glBuf = glProfile->glSum[i+dist];
          }
        }
        if ( shift != 0 ) {
          regPeakProfile->glSum[i+shift] = regPeakProfile->glSum[i];
          regPeakProfile->glSum[i] *= -1;
          printf( "> peak at x-position %d was shifted %d pixels\n", i, shift );
        }
        
	// old approach : walk whichever side goes down until bottom is reached
        //printf( "regPeak[%d]=%d gl[i-1,i,i+1]=%d,%d,%d\n", i, regPeakProfile->glSum[i], glProfile->glSum[i-1], glProfile->glSum[i], glProfile->glSum[i+1] ); 
	/*glBuf = glProfile->glSum[i];
	dist = 1;
	if ( glProfile->glSum[i-dist] < glBuf ) {
	  glBuf = glProfile->glSum[i+dist];
	  while ( glProfile->glSum[i-dist-1]<glBuf && dist<relaxDist ) {
	    dist++;
	    glBuf = glProfile->glSum[i-dist];
	  }
	  regPeakProfile->glSum[i-dist] = regPeakProfile->glSum[i];
	  regPeakProfile->glSum[i] *= -1;
	  printf( "> %d to the left\n", dist );
	}
	else if ( glProfile->glSum[i+dist] < glBuf ) {
	  glBuf = glProfile->glSum[i+dist];
	  while ( glProfile->glSum[i+dist+1]<glBuf && dist<relaxDist ) {
	    dist++;
	    glBuf = glProfile->glSum[i+dist];
	  }
	  regPeakProfile->glSum[i+1] = regPeakProfile->glSum[i];
	  regPeakProfile->glSum[i] *= -1;
	  printf( "> %d to the right\n", dist );
	}*/
        
        // even older unused stuff
  /*
	if ( glProfile->glSum[i-1] < glProfile->glSum[i+1] ) {
	  if ( glProfile->glSum[i-1] < glProfile->glSum[i] ) {
	    regPeakProfile->glSum[i-1] = regPeakProfile->glSum[i];
	    regPeakProfile->glSum[i] *= -1;
	  }
	}
	else if ( glProfile->glSum[i+1] < glProfile->glSum[i] ) {
	  regPeakProfile->glSum[i+1] = regPeakProfile->glSum[i];
	  regPeakProfile->glSum[i] *= -1;
	}
  */
      }
    }
  }

  return status;
}

int count_peaks_glProfile( glProfile_t *profile, int *nPeaks ) {
  int status = 0;

    int threshold = 0;
    int i;
    int n;

  if ( profile->glSum==NULL || profile->memState!=1 ) {
    status = -1;
    printf( "ERROR: count_peaks_glProfile(): Invalid argument 'profile->glSum==NULL || profile->memState!=1'\n" );
  }

    n = 0;
    //printf( "initially: nPeaks = %d\n", n );
    //printf( "i\tglSum[i]\n" );
    //printf( "%d\t%d\n", i, profile->glSum[i] );
    for ( i=0; i<profile->length; i++ ) {
    //scanf( "%d", nPeaks );
    //printf( "nPeaks = %d\n", *nPeaks );
        //printf( "% 3d: % 5d", i, profile->glSum[i] );
        if ( profile->glSum[i] > threshold ) {
            //printf( "\t\t+ Yup that's a minimum.\n" );
            n++;
        }
        else {
            //printf( "\t\t- Nope, that's none.\n" );
        }
    }
    //printf( "finally: nPeaks = %d\n", n );
    *nPeaks = n;


  return status;
}


int get_peakPositions( glProfile_t *profile, positionList_t *peakPos ) {
    int status = 0;
    
    int threshold = 0;
    int i;
    int cooOffset;
    int peakNo;
    int nIgnoredPeaks;
    
    if ( profile->glSum==NULL || profile->memState!=1 ) {
        status = -1;
        printf( "ERROR: store_peakPositions(): Invalid argument 'profile->glSum==NULL || profile->memState!=1'.\n" );
    }
    else if ( peakPos->list==NULL || peakPos->memState!=1 ) {
        status = -1;
        printf( "ERROR: store_peakPositions(): Invalid argument 'peakPos->list==NULL || peakPos->memState!=1'.\n" );
    }
    else if ( profile->direction!=0 && profile->direction!=1 ) {
        status = -1;
        printf( "ERROR: store_peakPositions(): Invalid argument 'profile->direction!=0 && profile->direction!=1'.\n" );
    }
    
    if ( status == 0 ) {
        if ( profile->direction == 0 ) {
            cooOffset = profile->p.x;
        }
        else if ( profile->direction == 1 ) {
            cooOffset = profile->p.y;
        }
        i = 0;
        peakNo = 0;
        printf( "Looking for %d peaks along a profile of %d pixels length\n", peakPos->length, profile->length );
        while ( peakNo<peakPos->length && i<profile->length ) {
            if ( profile->glSum[i] > threshold ) {
                peakPos->list[peakNo] = i+cooOffset;
                peakNo++;
            }
            i++;
            
        }
        nIgnoredPeaks = 0;
        if ( i<profile->length ) {
            while ( i<profile->length ) {
                if ( profile->glSum[i] > threshold ) {
                    nIgnoredPeaks++;
                }
                i++;
            }
            if ( nIgnoredPeaks > 0 ) {
                printf( "WARNING: get_peakPositions(): peak position list too short, ignoring %d peaks.\n", nIgnoredPeaks );
            }
        }
        if ( peakNo < peakPos->length ) {
            printf ( "WARNING: get_peakPositions(): Peak position list too long: 'peakNo=%d < peakPos->length=%d'.\n", peakNo, peakPos->length );
            for ( i=peakNo; i<peakPos->length; i++ ) {
                peakPos->list[i] = -1;
            }
        }
    }
    
    return status;
}


int correct_oddPeakPositions( positionList_t *peakPos ) {
  int status = 0;

  int i, ii;
  float meanPeakSpacing;
  float *peakEnergy = NULL;
  int peakEnergy_memState = 0;
  int range;
  int nEnergies;
  float meanEnergy;
  float minEnergy;
  float maxEnergy;

  int nTrustworthyPeaks;

  int dist_l, dist_r;
  int diff_lr;

  meanPeakSpacing = (float)(peakPos->list[peakPos->length-1] - peakPos->list[0])/(peakPos->length - 1);

  printf( "meanPeakSpacing = %f\n", meanPeakSpacing );

  for ( i=1; i<peakPos->length-1; i++ ) {
    dist_l = peakPos->list[i] - peakPos->list[i-1];
    dist_r = peakPos->list[i+1] - peakPos->list[i];
    //printf( " peak id i=%d : dist_l=%d dist_r=%d\n", i, dist_l, dist_r );
    if ( (dist_l+dist_r-2*meanPeakSpacing) < 1.5 ) {
      diff_lr = dist_l - dist_r;
      if ( diff_lr < 0 ) {
	diff_lr *= -1;
      }
      if ( diff_lr > 2 ) {
	printf( "raster line at xPos = %d is odd one out of its left and right neighbors\n", peakPos->list[i] );
	printf( "It is put at xPos' = %d and peak with id %d is jumped over\n", (int)((peakPos->list[i+1] + peakPos->list[i-1])/2.0 + 0.5), i+1 );
	peakPos->list[i] = (int)((peakPos->list[i+1] + peakPos->list[i-1])/2.0 + 0.5);
	i++;
      } 
    }
  }
  


  //float eBuf;
  //int i_left, i_right;

  //float EHbinSize;
  //float energyHistogram[2][30];

  //FILE *file;
  //char *fileName = "peakEnergies.txt" ;

  //file = fopen( fileName, "w" );

  if ( (peakEnergy = malloc( (peakPos->length)*sizeof(float) )) == NULL ) {
    status = -1;
    printf( "ERROR: correct_oddPeakPositions(): Could not allocate memory for 'peakEnergy'.\n" );
  } else {
    peakEnergy_memState = 1;
  }

    /*
  if ( status == 0 ) {
    / *
    printf( ">>>>> Choose a range for neighbors contributing to the energy of a peak.\n" );
    printf( "(int)range = " );
    scanf( "%d", &range );
    * /
    range = 7;
    nEnergies = peakPos->length - 2*range;
    printf( "> Now computing and storing energies for %d peaks with id ranging from %d to %d.\n", nEnergies, range, peakPos->length-1-range );
    meanPeakSpacing = (float)(peakPos->list[peakPos->length-1-range]-peakPos->list[range])/(nEnergies-1);
    //fprintf( file, "peak id\t peak pos\t peak energy (meanPeakPacing = %f)\n", meanPeakSpacing );
    for ( i=0; i<range; i++ ) {
      peakEnergy[i] = peakEnergy[peakPos->length-1-i] = -1;
    }
    meanEnergy = 0;
    minEnergy = 2*range*meanPeakSpacing*meanPeakSpacing;
    maxEnergy = -1;
    //printf( "Peak Energies: " );
    for ( i=range; i<peakPos->length-range-1; i++ ) {
      peakEnergy[i] = 0;
      for ( ii=-range; ii<=range; ii++ ) {
	peakEnergy[i] += (float)(peakPos->list[i]+ii*meanPeakSpacing - peakPos->list[i+ii])*(peakPos->list[i]+ii*meanPeakSpacing - peakPos->list[i+ii]); 
      }
      //fprintf( file, "%d\t%d\t%f\n", i, peakPos->list[i], peakEnergy[i] );
      meanEnergy += peakEnergy[i];
      if ( peakEnergy[i] < minEnergy ) {
	minEnergy = peakEnergy[i];
      }
      if ( peakEnergy[i]> maxEnergy ) {
	maxEnergy = peakEnergy[i];
      }
    }
    //fclose( file );
    //printf( ">>>>>>>>>>>>>>>> Stored list with peak id, peak pos and peak energy to '%s'\n", fileName );
    meanEnergy /= nEnergies;
    printf( "meanEnergy = %f\n", meanEnergy );
    printf( "minEnergy = %f\n", minEnergy );
    printf( "maxEnergy = %f\n", maxEnergy );


    printf( "looking for trustworthiest peaks...\n" );
    printf( "xPos\n" );
    nTrustworthyPeaks = 0;
    for ( i=range; i<peakPos->length-range-1; i++ ) {
      if ( peakEnergy[i] < meanEnergy ) {
	printf( "%d\n", peakPos->list[i] );
	nTrustworthyPeaks++;
      }
    }
    printf( "%d/%d trustworthy peaks found\n", nTrustworthyPeaks, nEnergies );

    / *
    if ( maxEnergy > 5*meanEnergy ) {
      printf( "> 'maxEnergy > 5*meanEnergy' --> going through ratser to correct odd raster points\n" );

      for ( i=range; i<peakPos->length-range-1; i++ ) {

	if ( peakEnergy[i] > 5*meanEnergy ) {
	  printf( "Odd peak at: xPos = %d\n", peakPos->list[i] );
	  eBuf = peakEnergy[i];
	  for ( ii=-range; ii<0; ii++ ) {
	    if ( peakEnergy[i+ii] < eBuf ) {
	      i_left = i+ii;
	      eBuf = peakEnergy[i_left];
	    }
	  }
	  printf( "peak with lowest energy to the left withing range found at: i_left = %d\n", i_left );
	  eBuf = peakEnergy[i];
	  for ( ii=1; ii<=range; ii++ ) {
	    if ( peakEnergy[i+ii] < eBuf ) {
	      i_right = i+ii;
	      eBuf = peakEnergy[i_right];
	    }
	  }
	  printf( "peak with lowest energy to the right withing range found at: i_right = %d\n", i_right );
	  printf( "extrapolated xPos = %d, yields peakEnergy = %f\n", peakPos->list[i_right]-(int)(meanPeakSpacing*(i_right-i)+0.5), -1.0*i_right ); 
	}

      }

      }* /

    / *
    printf( "> computing energy histogram using 30 equally spaced bins...\n" );
    EHbinSize = (maxEnergy - minEnergy)/30; 
    for ( i=0; i<30; i++ ) {
      energyHistogram[0][i] = minEnergy + (i+0.5)*EHbinSize;
      energyHistogram[1][i] = 0;
    }
    * /
    /*printf( "Here is the histogram of the peak energies:\n" );
    for ( i=0; i<30; i++ ) {
      printf( "%f\t%f\n", energyHistogram[0][i], energyHistogram[1][i]);
    }
    printf( "\n" );
    * /
    / *
    for ( i=range; i<peakPos->length-range-1; i++ ) {
      printf( "(int)((peakEnergy[i]-minEnergy)/EHbinSize) = %d\n", (int)((peakEnergy[i]-minEnergy)/EHbinSize) );
      energyHistogram[1][(int)((peakEnergy[i]-minEnergy)/EHbinSize)] +=1;
    }
  

    printf( "Here is the histogram of the peak energies:\n" );
    printf( "[" );
    for ( i=0; i<30; i++ ) {
      printf( "[%f,%f];", energyHistogram[0][i], energyHistogram[1][i]);
    }
    printf( "]\n" );
    * /
  }
  */
  if ( peakEnergy_memState == 1 ) {
    free( peakEnergy );
  }

  return status;
}


// *** reconstructing the colors ***
// *********************************

int reconstruct_colorFrame( glImage_t *glScan, positionList_t *rasterPos, rgbImage_t *rgbFrame ) {
    int status = 0;
    
    int xCol, yCol;
    int x, y, xx, yy;
    int channel;
    int buf;
  int s;
  int enveloppe[1] = {1};//[3] = {1,2,1}; //[5] = {1,3,4,3,1};//[7] = {1,2,7,8,7,2,1};
  int enveloppeLength = 1;
  int enveloppeCenter = 0;
  int xBinSize, yBinSize;
    int rasterOffset;
    int color[3];
    float chSpace;
    int backgroundGl = 0;
    
    float colTrsf[3][3] = {{1.021277, -0.148936, 0.021277},{-0.148936, 1.042553, -0.148936},{0.021277, -0.148936, 1.021277}};
    float col[3];
    int i;
    int j;
    int glPoint[3];
    
    int dist;
    int gls[3];
    
    int xPos;
  int rgbFrameWidth;
    
    //int separatorWidth = 5; // to be a tunable parameter
    
    if ( glScan->img==NULL || glScan->memState!=1 ) {
        status = -1;
        printf( "ERROR: reconstruct_colorFrame(): Invalid argument 'glScan->img==NULL || glScan->memState!=1'\n" );
    } 
    else if ( rasterPos->list==NULL || rasterPos->memState!=1 ) {
        status = -1;
        printf( "ERROR: reconstruct_colorFrame(): Invalid argument 'rasterPos->list==NULL || rasterPos->memState!=1'\n" );
    } 
    else if ( rgbFrame->img==NULL || rgbFrame->memState!=1 ) {
        status = -1;
        printf( "ERROR: reconstruct_colorFrame(): Invalid argument 'rgbFrame->img==NULL || rgbFrame->memState!=1'\n" );
    } 
    else if ( rgbFrame->width != rasterPos->length - 1 ) {
        status = -1;
        printf( "ERROR: reconstruct_colorFrame(): Incompatible arguments 'rgbFrame->width!=rasterPos->length-1'\n" );
    } 
    
    // add an argument 'oversampling' to specify how many pixels per dimension one raster step shall be, i.e. at the moment I do a hard coded version using what then would be 'oversampling = 1' 
    
    
    // determine how many lines are to be summed up to compute the color of a pixel in the rgbFrame
    
    if ( status == 0 ) {
      rasterOffset = 0;
      yBinSize = glScan->height/rgbFrame->height;
      printf( "yBinSize = %d\n", yBinSize );
      rgbFrameWidth = rgbFrame->width-1;
      printf( "> using 'rgbFrameWidth' = %d to mirror frame horizontally\n", rgbFrameWidth ); 
      
    }
    if ( status == 0 ) {
        for ( xCol=0; xCol<rgbFrame->width; xCol++ ) {
            x = rasterPos->list[xCol];
            chSpace = (rasterPos->list[xCol+1] - x)/4.0;
            for ( yCol=0; yCol<rgbFrame->height; yCol++ ) {
                y = yBinSize*yCol;
                for ( i=0; i<3; i++ ) {
                    gls[i] = 0;
                  xPos = x+(int)((i+1)*chSpace);
                    //printf ( "i = %d\t xPos = %d\t", i, xPos );
                    for ( yy=0; yy<yBinSize; yy++ ) {
                      for ( s=0; s<enveloppeLength; s++ ) {
                        gls[i] += enveloppe[s]*(glScan->img[y+yy][xPos+s-enveloppeCenter]);
                      }
                    }
                    //printf( "gls[%d] = %d\n", i, gls[i]);
                }
                for ( channel=0; channel<3; channel++ ) {
                    col[channel] = 0;
                    for ( i=0; i<3; i++ ) {
                        col[channel] += colTrsf[channel][i]*gls[i];
                    }
                    if ( col[channel] < 0 ) {
                        rgbFrame->img[channel][yCol][rgbFrameWidth-xCol] = 0;
                    }
                    else {
                        rgbFrame->img[channel][yCol][rgbFrameWidth-xCol] = (int)(col[channel]);   
                    }
                }
            }
        }
    }
    
    return status;
}


int reconstructInterpolate_colorFrame( glImage_t *glScan, positionList_t *rasterPos, rgbImage_t *rgbFrame ) {
  int status = 0;
  
  int xCol, yCol;
  int x, y, xx, yy;
  int rgbFrameWidth;
  int channel;
  float buf;
  int s;
  int enveloppe[1] = {1};//[3] = {1,2,1}; //[5] = {1,3,4,3,1};//[7] = {1,2,7,8,7,2,1};
  int enveloppeLength = 1;
  int enveloppeCenter = 0;
  int xBinSize, yBinSize;
  int rasterOffset;
  int color[3];
  float chSpace;
  int backgroundGl = 0;
  float relD[3];
  int intDir;
  
  float colTrsf[3][3] = {{1.021277, -0.148936, 0.021277},{-0.148936, 1.042553, -0.148936},{0.021277, -0.148936, 1.021277}};
  float col[3];
  int i;
  int j;
  int glPoint[3];
  
  int dist;
  int gls[3];
  
  int xPos;
  
  int separatorWidth = 5; // to be a tunable parameter
  
  if ( glScan->img==NULL || glScan->memState!=1 ) {
    status = -1;
    printf( "ERROR: reconstructInterpolate_colorFrame(): Invalid argument 'glScan->img==NULL || glScan->memState!=1'\n" );
  } 
  else if ( rasterPos->list==NULL || rasterPos->memState!=1 ) {
    status = -1;
    printf( "ERROR: reconstructInterpolate_colorFrame(): Invalid argument 'rasterPos->list==NULL || rasterPos->memState!=1'\n" );
  } 
  else if ( rgbFrame->img==NULL || rgbFrame->memState!=1 ) {
    status = -1;
    printf( "ERROR: reconstructInterpolate_colorFrame(): Invalid argument 'rgbFrame->img==NULL || rgbFrame->memState!=1'\n" );
  } 
  else if ( rgbFrame->width != 2*(rasterPos->length - 1) - 1 ) {
    status = -1;
    printf( "ERROR: reconstructInterpolate_colorFrame(): Incompatible arguments 'rgbFrame->width != 2*(rasterPos->length-1)-2'\n" );
  } 
  
  // determine how many lines are to be summed up to compute the color of a pixel in the rgbFrame
  if ( status == 0 ) {
    rasterOffset = 0;
    yBinSize = glScan->height/rgbFrame->height;
    printf( "> yBinSize = %d\n", yBinSize );
    rgbFrameWidth = rgbFrame->width-1;
    printf( "> using 'rgbFrameWidth' = %d to mirror frame horizontally\n", rgbFrameWidth ); 
    
  }
  if ( status == 0 ) {
    for ( xCol=0; xCol<rgbFrame->width; xCol+=2 ) {
      x = rasterPos->list[xCol/2]; 
      chSpace = (rasterPos->list[xCol/2+1] - x)/4.0;
      for ( yCol=0; yCol<rgbFrame->height; yCol++ ) {
        y = yBinSize*yCol;
        for ( i=0; i<3; i++ ) {
          gls[i] = 0;
          xPos = x+(int)((i+1)*chSpace);
          for ( yy=0; yy<yBinSize; yy++ ) {
            for ( s=0; s<enveloppeLength; s++ ) {
              gls[i] += enveloppe[s]*(glScan->img[y+yy][xPos+s-enveloppeCenter]);
            }
          }
        }
        for ( channel=0; channel<3; channel++ ) {
          col[channel] = 0;
          for ( i=0; i<3; i++ ) {
            col[channel] += colTrsf[channel][i]*gls[i];
          }
          if ( col[channel] < 0 ) {
            rgbFrame->img[channel][yCol][rgbFrameWidth-xCol] = 0;
            //rgbFrame->img[channel][yCol][xCol] = 0;
          }
          else {
            rgbFrame->img[channel][yCol][rgbFrameWidth-xCol] = (int)(col[channel]);
            //rgbFrame->img[channel][yCol][xCol] = (int)(col[channel]);
          }
        }
      }
    }
    /*
    // channel specific 3 directional interpolation
    for ( channel=0; channel<3; channel++ ) {
      for ( xCol=1; xCol<rgbFrame->width-1; xCol+=2 ) {
        rgbFrame->img[channel][0][xCol] = (int)((rgbFrame->img[channel][0][xCol-1]+rgbFrame->img[channel][0][xCol+1])/2.0 + 0.5);
        for ( yCol=1; yCol<rgbFrame->height-1; yCol++ ) {
          
          relD[0] = ((float)(rgbFrame->img[channel][yCol][xCol-1] - rgbFrame->img[channel][yCol][xCol+1]))/(rgbFrame->img[channel][yCol][xCol-1] + rgbFrame->img[channel][yCol][xCol+1]);
          if (relD[0] < 0 ) {
            relD[0] *= -1;
          }
          relD[1] = ((float)(rgbFrame->img[channel][yCol-1][xCol-1] - rgbFrame->img[channel][yCol+1][xCol+1]))/(rgbFrame->img[channel][yCol-1][xCol-1] + rgbFrame->img[channel][yCol+1][xCol+1]);
          if (relD[1] < 0 ) {
            relD[1] *= -1;
          }
          relD[2] = ((float)(rgbFrame->img[channel][yCol+1][xCol-1] - rgbFrame->img[channel][yCol-1][xCol+1]))/(rgbFrame->img[channel][yCol+1][xCol-1] + rgbFrame->img[channel][yCol-1][xCol+1]);
          if (relD[2] < 0 ) {
            relD[2] *= -1;
          }
          intDir = 0;
          if ( relD[1] < relD[0] ) {
            intDir = 1;
            if ( relD[2] < relD[1] ) {
              intDir = 2;
            }            
          } else if ( relD[2] < relD[0] ) {
            intDir = 2;
          }
          intDir = 0;
          if ( intDir == 0 ) {
            rgbFrame->img[channel][yCol][xCol] = (int)((rgbFrame->img[channel][yCol][xCol-1]+rgbFrame->img[channel][yCol][xCol+1])/2.0 + 0.5);
          }
          else if ( intDir == 1 ) {
            //printf( ">>>>>> intDir == 1\n" );
            rgbFrame->img[channel][yCol][xCol] = (int)((rgbFrame->img[channel][yCol-1][xCol-1]+rgbFrame->img[channel][yCol+1][xCol+1])/2.0 + 0.5);
          }
          else {
            //printf( ">>>>>> intDir == 1\n" );
            rgbFrame->img[channel][yCol][xCol] = (int)((rgbFrame->img[channel][yCol+1][xCol-1]+rgbFrame->img[channel][yCol-1][xCol+1])/2.0 + 0.5);
          }
           
          //rgbFrame->img[channel][yCol][xCol] = 0;
        }
        rgbFrame->img[channel][rgbFrame->height-1][xCol] = (int)((rgbFrame->img[channel][rgbFrame->height-1][xCol-1]+rgbFrame->img[channel][rgbFrame->height-1][xCol+1])/2.0 + 0.5);
      }
    }
    */
    // color specific 3 directional interpolation
    for ( channel=0; channel<3; channel++ ) {
      for ( xCol=1; xCol<rgbFrame->width-1; xCol+=2 ) {
        rgbFrame->img[channel][0][xCol] = (int)((rgbFrame->img[channel][0][xCol-1]+rgbFrame->img[channel][0][xCol+1])/2.0 + 0.5);
        rgbFrame->img[channel][rgbFrame->height-1][xCol] = (int)((rgbFrame->img[channel][rgbFrame->height-1][xCol-1]+rgbFrame->img[channel][rgbFrame->height-1][xCol+1])/2.0 + 0.5);
      }
    }
    for ( yCol=1; yCol<rgbFrame->height-1; yCol++ ) {
      for ( xCol=1; xCol<rgbFrame->width-1; xCol+=2 ) {
        relD[0] = relD[1] = relD[2] = 0;
        for ( channel=0; channel<3; channel++ ) {
          buf = ((float)(rgbFrame->img[channel][yCol][xCol-1] - rgbFrame->img[channel][yCol][xCol+1]))/(rgbFrame->img[channel][yCol][xCol-1] + rgbFrame->img[channel][yCol][xCol+1]);
          //printf( "buf0=%f ", buf );
          if ( buf < 0 ) {
            relD[0] -= buf;
          } else {
            relD[0] += buf;
          }
          buf = ((float)(rgbFrame->img[channel][yCol-1][xCol-1] - rgbFrame->img[channel][yCol+1][xCol+1]))/(rgbFrame->img[channel][yCol-1][xCol-1] + rgbFrame->img[channel][yCol+1][xCol+1]);
          if ( buf < 0 ) {
            relD[1] -= buf;
          } else {
            relD[1] += buf;
          }
          buf = ((float)(rgbFrame->img[channel][yCol+1][xCol-1] - rgbFrame->img[channel][yCol-1][xCol+1]))/(rgbFrame->img[channel][yCol+1][xCol-1] + rgbFrame->img[channel][yCol-1][xCol+1]);
          if ( buf < 0 ) {
            relD[2] -= buf;
          } else {
            relD[2] += buf;
          }
        }
        intDir = 0;
        if ( relD[1] < relD[0] ) {
          intDir = 1;
          if ( relD[2] < relD[1] ) {
            intDir = 2;
          }            
        } else if ( relD[2] < relD[0] ) {
          intDir = 2;
        }
        if ( intDir == 0 ) {
          for ( channel=0; channel<3; channel++ ) {
            rgbFrame->img[channel][yCol][xCol] = (int)((rgbFrame->img[channel][yCol][xCol-1]+rgbFrame->img[channel][yCol][xCol+1])/2.0 + 0.5);
          }
          //rgbFrame->img[1][yCol][xCol] = 400000;
        }
        else if ( intDir == 1 ) {
          for ( channel=0; channel<3; channel++ ) {
            rgbFrame->img[channel][yCol][xCol] = (int)((rgbFrame->img[channel][yCol-1][xCol-1]+rgbFrame->img[channel][yCol+1][xCol+1])/2.0 + 0.5);
          }
          //rgbFrame->img[0][yCol][xCol] = 450000;
        }
        else {
          for ( channel=0; channel<3; channel++ ) {
            rgbFrame->img[channel][yCol][xCol] = (int)((rgbFrame->img[channel][yCol+1][xCol-1]+rgbFrame->img[channel][yCol-1][xCol+1])/2.0 + 0.5);
          }
          //rgbFrame->img[2][yCol][xCol] = 450000;
        }
      }
    }
  }
  
  return status;
}


int pickColor_interactively( glImage_t *glScan, positionList_t *rasterPos ) {
  int status = 0;

  int rgbRasterWidth, rgbRasterHeight;
  int rgbX, rgbY;
  int yBinSize;
  int glCurve[32];
  int x, y;
  int xx, yy;
  int count;
  int dist;
    int r, g, b;
    int backgroundGl;
    int enveloppe[7] = {1,2,7,8,7,2,1};
    int envLen = 7;
    float overlap = 1.0/7;
    float colTrsf[3][3] = {{1.021277, -0.148936, 0.021277},{-0.148936, 1.042553, -0.148936},{0.021277, -0.148936, 1.021277}};
    float col[3];
    int i;
    int channel;
    int glPoint[3];
    

  if ( glScan->img==NULL || glScan->memState!=1 ) {
    status = -1;
    printf( "ERROR: pickColor_interactively(): Invalid argument 'glScan->img==NULL || glScan->memState!=1'\n" );
  } 
  else if ( rasterPos->list==NULL || rasterPos->memState!=1 ) {
    status = -1;
    printf( "ERROR: pickColor_interactively(): Invalid argument 'rasterPos->list==NULL || rasterPos->memState!=1'\n" );
  }
  
  if ( status == 0 ) {
    rgbRasterWidth = rasterPos->length;
    rgbRasterHeight = (glScan->height*rgbRasterWidth)/(glScan->width);
    yBinSize = (glScan->height)/rgbRasterHeight;
    printf( "\n______ interactive mode _______\n" );
    printf( "dimensions of rgb frame: %d x %d\n", rgbRasterWidth, rgbRasterHeight );
    printf( "yBinSize = %d, xBinSize = dist between adjacent raster lines\n", yBinSize );
    printf( "\n" );
    printf( "Enter coordinates within the above reported range to see\n" );
    printf( "A) the gl profile and\n" );
    printf( "B) get the color reconstructed therefrom.\n" );
    printf( "to exit enter '-1 -1' / three times in a row coordinates outside admissible range will force exit as will non int input\n" );

      
    backgroundGl = 1000;  
    printf( "bkgrnd = %d\n", backgroundGl );
      
      
    rgbX = rgbY = -2;
    count = 0;
    while ( !(rgbX==-1 && rgbY==-1) && count<3 ) {
      printf( "rgbX = " );
      scanf( "%d", &rgbX );
      printf( "rgbY = " );
      scanf( "%d", &rgbY );

      if ( !(rgbX<rgbRasterWidth) || !(rgbY<rgbRasterHeight) ) {
	count++;
	printf( "position out of rgb frame\n" );
	printf( "Enter coordinates within the above reported range to see\n" );
	printf( "A) the gl profile and\n" );
	printf( "B) get the color reconstructed therefrom.\n" );
	printf( "to end enter '-1 -1'\n" ); 
      } else if ( !(rgbX>-1) || !(rgbY>-1) ) {
	count++;
	printf( "position out of rgb frame\n" );
	printf( "Enter coordinates within the above reported range to see\n" );
	printf( "A) the gl profile and\n" );
	printf( "B) get the color reconstructed therefrom.\n" );
	printf( "to end enter '-1 -1'\n" ); 
      }
      else if ( rgbX!=-1 && rgbY!=-1 ) {
	x = rasterPos->list[rgbX];
	y = rgbY*yBinSize;
	printf( "> (%d,%d)rgbFrame  <->  (%d,%d)glScan\n", rgbX, rgbY, x, y );
	dist = rasterPos->list[rgbX+1] - x + 1;
	if ( dist<32 ) {
	  for ( xx=0; xx<dist; xx++ ) {
	    glCurve[xx] = 0;
	    for ( yy=0; yy<yBinSize; yy++ ) {
	      glCurve[xx] += glScan->img[y+yy][x+xx];
	    }
	  }
	  printf( "Here is the gl curve:\n" );
	  for ( xx=0; xx<dist; xx++ ) {
	    printf( "% 6d ", glCurve[xx] );
	  }
	  printf( "\n" );
        //printf( "positions: dist/4=%d, dist/2=%d, 3*dist/4=%d\n", dist/4, dist/2, 3*dist/4 );
        /*
        for ( xx=0; xx<dist/4; xx++ ) {
            printf( "   -   " );
        }
        printf( "  -r-  " );
        for ( xx=dist/4+1; xx<dist/2; xx++ ) {
            printf( "   -   " );
        }
        printf( "  -g-  " );
        for ( xx=dist/2+1; xx<3*dist/4; xx++ ) {
            printf( "   -   " );
        }
        printf ( "  -b- \n" );
        */
        
        for ( channel=0; channel<3; channel++ ) {
            glPoint[channel] = glCurve[(channel+1)*dist/4]-backgroundGl;
        }
        
        for ( i=0; i<3; i++ ) {
            for ( channel=0; channel<3; channel++ ) {
                col[i] += colTrsf[i][channel]*glPoint[channel];
            }
        }  
        r = (int)col[0];
        b = (int)col[1];
        g = (int)col[2];
        printf( "|r'|g'|b'| = |%d|%d|%d|\n", r, g, b );

        
	}
	else {
	  printf( "TOO large distance to next raster, ingnoring color pick request\n" );
	}
      }
    }
  }

  return status;
}



    // rather geometry stuff...
// *** localizing the lens array ***
// *********************************

int new_positionList( positionList_t *positions, int length ) {
  int status = 0;

  if ( positions->list!=NULL || positions->memState!=0 ) {
    status = -1; 
    printf( "ERROR: new_positionList(): Invalid argument 'positions->list!=NULL || positions->memState!=0'\n" );
  }
  else if ( length < 0 ) {
    status = -1; 
    printf( "ERROR: new_positionList(): Invalid argument 'length<0'\n" );
  }

  if ( status == 0 ) {
    positions->length = length;
    if ( (positions->list = malloc( length*sizeof(int) )) == NULL ) {
      status = -1;
      printf( "ERROR: new_positionList(): Failed to allocate memory for 'positions->list'.\n" );
    }
    if ( status == 0 ) {
      positions->memState = 1;
    } 
    else {
      positions->memState = -1;
    }
  }

  return status;
}

int delete_positionList( positionList_t *positions ) {
  int status = 0;

  if ( positions->list==NULL || positions->memState==0  ) {
    status = -1;
    printf( "ERROR: delete_positionList(): Cannot free memory 'positions->list==NULL || positions->memState==0'\n" );
  } 
  else if ( positions->length < 0 ) {
    status = -1; 
    printf( "ERROR: delete_positionList(): Invalid argument 'positions->length<0'\n" );
  }

  if ( status == 0 ) {
    free( positions->list );
    positions->list = NULL;
    positions->memState = 0;
  }

  return status;
}

// *** drawing into image ***
// **************************

int draw_verticalLines_rgbImage( rgbImage_t *rgbImage, positionList_t *positions, int color[3] ) {
  int status = 0;

  int i;
  int x, x_max;
  int y, y_max;
  int c;

  if ( rgbImage->img==NULL || rgbImage->memState!=1 ) {
    status = -1;
    printf( "ERROR: draw_verticalLines_rgbImage(): Invalid argument 'rgbImage->img==NULL || rgbImage->memState!=1'\n" );
  } 
  else if ( positions->list==NULL || positions->memState!=1 ) {
    status = -1;
    printf( "ERROR: draw_verticalLines_rgbImage(): Invalid argument 'positions->list==NULL || positions->memState!=1'\n" );
  } 
  else if ( color[0]<0 || color[0]>255 || color[1]<0 || color[1]>255 || color[2]<0 || color[2]>255 ) {
    status = -1;
    printf( "ERROR: draw_verticalLines_rgbImage(): Invalid argument 'color[0]<0 || color[0]>255 || color[1]<0 || color[1]>255 || color[2]<0 || color[2]>255'\n" );
  } 
    
    //printf( "Wanna draw lines now ..... status = %d\n", status );

  if ( status == 0 ) {
    x_max = rgbImage->width-1;
    y_max = rgbImage->height-1;
    for ( i=0; (i<positions->length)&&(status==0); i++ ) {
      x = positions->list[i];  
        //printf( "line %d at x = %d\n", i, x );
      if ( x>=0 && x<=x_max ) {
	for ( y=0; y<=y_max; y++ ) {
	  for ( c=0; c<3; c++ ) {
	    rgbImage->img[c][y][x] = color[c];
	  }
	} 
      }
      else {
	printf( "WARNING: draw_verticalLines_rgbImage(): Skipping drawing of line, because line is outside of image x=%d not within x_min=0, x_max=%d \n", x, x_max );
      }
    }
  }

  return status;
}

int draw_raster_rgbImage( rgbImage_t *rgbImage, glProfile_t *profile, positionList_t *positions ) {
int status = 0;

  int i;
  int x, x_max;
  int y, y_max;
  int c;

  if ( rgbImage->img==NULL || rgbImage->memState!=1 ) {
    status = -1;
    printf( "ERROR: draw_raster_rgbImage(): Invalid argument 'rgbImage->img==NULL || rgbImage->memState!=1'\n" );
  } 
  else if ( positions->list==NULL || positions->memState!=1 ) {
    status = -1;
    printf( "ERROR: draw_raster_rgbImage(): Invalid argument 'positions->list==NULL || positions->memState!=1'\n" );
  } 
  else if ( profile->glSum==NULL || profile->memState!=1 ) {
    status = -1;
    printf( "ERROR: draw_raster_rgbImage(): Invalid argument 'profile->glSum==NULL || profile->memState!=1'\n" );
  } 
    
  printf( "Wanna draw lines now with the color set by the kind of the line, i.e. extracted or reconstructed\n" );

  if ( status == 0 ) {
    x_max = rgbImage->width-1;
    y_max = rgbImage->height-1;
    for ( i=0; (i<positions->length)&&(status==0); i++ ) {
      x = positions->list[i];  
      //printf( "line %d at x = %d\n", i, x );
      if ( x>=0 && x<=x_max ) {
        if ( profile->glSum[x] == 1  ) {
          for ( y=0; y<=y_max; y++ ) {
            rgbImage->img[0][y][x] /= 3;
            rgbImage->img[1][y][x] /= 3;
            rgbImage->img[2][y][x] = 255;
          }
        }
        else if ( profile->glSum[x] == 2  ) {
          for ( y=0; y<=y_max; y++ ) {
            rgbImage->img[0][y][x] = 255;
            rgbImage->img[1][y][x] /= 3;
            rgbImage->img[2][y][x] = 255;
          }
        }
        else if ( profile->glSum[x] == 3 ) {
          for ( y=0; y<=y_max; y++ ) {
            rgbImage->img[0][y][x] /= 3;
            rgbImage->img[1][y][x] = 255;
            rgbImage->img[2][y][x] /= 3;
          }	
        } 
        else {
          printf( "WARNING: draw_raster_rgbImage(): Missing line: 'profile->glSum[%d] = %ld'\n", x, profile->glSum[x] );
        }
      }
      else {
        printf( "WARNING: draw_raster_rgbImage(): Skipping drawing of line, because line is outside of image x=%d not within x_min=0, x_max=%d \n", x, x_max );
      }
    }
  }

  return status; 
}


/*
//
//
// write a gray level image to the disk as TIFF, format: 8-bit gray level
int bitstore_glImage2TIFF( bitstore_glImage_t *glImage, char *outTIFFname ){
  //
  // declare local variables
  TIFF *outTIFF;
  int j,i;
  unsigned char *lineBuffer;
  //
  // check if glImage actually was allocated
  if ( glImage->img == NULL ){
    printf( "ERROR: Can't save gl image to TIFF since its 'img' field isn't allocated!\n" );
    return (-1); 
  }
  //
  // check if range of gray levels fits for storing as 8-bit image
  int result = testIfRange0to255(glImage);
  if ( result != 1 ){
    printf( "ERROR: Can't save glImage: gray levels out of range! (found %d)\n", result );
    return (-1);
  }
  //
  // create TIFF file on disk
  if ( (outTIFF = TIFFOpen(outTIFFname, "w"))==NULL ){
    printf( "ERROR: Could not create '%s.\n'", outTIFFname );
    return (-1);
  }
  // write a minimal TIFF heather
  TIFFSetField( outTIFF, TIFFTAG_IMAGEWIDTH, glImage->nx );
  TIFFSetField( outTIFF, TIFFTAG_IMAGELENGTH, glImage->ny );
  TIFFSetField( outTIFF, TIFFTAG_BITSPERSAMPLE, 8 );
  TIFFSetField( outTIFF, TIFFTAG_SAMPLESPERPIXEL, 1);
  TIFFSetField( outTIFF, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
  TIFFSetField( outTIFF, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  //
  // write the image to the disk scanline by scanline
  MALLOC( lineBuffer, unsigned char, glImage->nx, return (-1) ); /// --????--- error handling OK ---????--
  for ( j=0; j<glImage->ny; j++ ){
    for ( i=0; i<glImage->nx; i++ ){
      lineBuffer[i] = (unsigned char) glImage->img[j][i];
    }
    if (TIFFWriteScanline (outTIFF, lineBuffer, j, 0) == -1) {
      (void) fprintf (stderr, "ERROR: Couldn't write scan line (%d)! (E#%d)\n", j, __LINE__);
      return (-1);
    }   
  }
  FREE( lineBuffer );
  //
  // close the TIFF to have it on the disk
  TIFFClose( outTIFF );
  //
  // report success
#ifdef _VERBOSE_OUTPUT
  printf( "bitstore_glImage2TIFF(): 1, Wrote %s to the disk.\n", outTIFFname );
#endif
  return 1;
}
//
//


 
// read a gray level TIFF and store it in a gray level image
int TIFF2bitstore_glImage( char *inTIFFname, bitstore_glImage_t *glImage ){
  //
  //declare local variables
  int j,i;
  TIFF *inTIFF;
  unsigned char *lineBuffer;
  short spp, bps;
    
  // size of image in pixels
  int ny, nx;
  //
  // open the TIFF for reading
#ifdef _VERBOSE_OUTPUT
  printf( "Opening %s.\n", inTIFFname );
#endif
  if ( (inTIFF = TIFFOpen(inTIFFname,"r")) == NULL ){
    (void) fprintf( stderr, "ERROR: Couldn't read image %s\n", inTIFFname );
    return (-1);
  }
  //
  // check if the image is gray level 8-bit
  TIFFGetField( inTIFF,TIFFTAG_SAMPLESPERPIXEL, &spp );
  TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
#ifdef _VERBOSE_OUTPUT
  printf( "spp = %d bps = %d\n", spp, bps );
#endif
  if ( spp!=1 || bps!=8 ){
    (void) fprintf( stderr, "ERROR: Image '%s' is not gray level 8-bit:\n\tsamples per pixel = %d\n\tbits per sample = %d\n\tWon't read it!\n", inTIFFname, spp, bps );
    return (-1);
  }
  //
  // set the dimensions of the glImage and allocate memory
  TIFFGetField( inTIFF,TIFFTAG_IMAGEWIDTH, &nx );
  TIFFGetField( inTIFF,TIFFTAG_IMAGELENGTH, &ny );
#ifdef _VERBOSE_OUTPUT
  printf( "width: nx = %d, length: ny = %d\n", nx, ny );
#endif
  if ( new_bitstore_glImage(glImage, nx, ny) == -1 ){
    return (-1);
  }
  //
  // read the image content scanline by scanline into the glImage
  MALLOC( lineBuffer, unsigned char, glImage->nx, {printf("ERROR: Can't allocate memory for buffer to hold image scan lines.\n"); return (-1);} );
  for ( j=0; j<glImage->ny; j++ ){
    // get a scanline
    if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ){
      (void) fprintf( stderr, "ERROR : Couldn't read scanline %d from %s.\n", j, inTIFFname );
      return (-1);
    }
    // transfer scanline to glImage
    for( i=0; i<glImage->nx; i++ ){
      glImage->img[j][i] = (int) lineBuffer[i];
    }
  }
  FREE( lineBuffer );
  //
  // report success
#ifdef _VERBOSE_OUTPUT
  printf( "TIFF2bitstore_glImage(): 1, Read %s and created a gl image in the heap.\n", inTIFFname );
#endif
  return 1;
}





// *****************
// *** rgb image ***
// *****************
//
//
// initialize a bistore_rgbImage_t


//
// read a rgb TIFF and store it in a rgb image
// 15.05.2012 / AW
int TIFF2bitstore_rgbImage( char *inTIFFname, bitstore_rgbImage_t *rgbImage ){
  int j,i,c;
  TIFF *inTIFF;
  unsigned char *lineBuffer;
  short spp, bps;
    
  // size of image in pixels
  int ny, nx;
    
  // open the TIFF for reading
  if ( (inTIFF = TIFFOpen(inTIFFname,"r")) == NULL ){
    (void) fprintf( stderr, "ERROR : Couldn't read image %s\n", inTIFFname );
    return (-1);
  }
    
  printf( "TIFF %s is open for reading.\n", inTIFFname );
  // check if the image is rgb 8-bit
  TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, &spp );
  TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, &bps );
  printf( "spp = %d bps = %d\n", spp, bps );
  if ( spp!=3 || bps!=8 ){
    (void) fprintf( stderr, "ERROR : Image '%s' is not rgb 8-bit, won't read it.\n", inTIFFname );
    return (-1);
  }
  //printf( "checked if TIFF file contains a gray level image\n" );
    
  // set the dimensions of the glImage and allocate memory
  TIFFGetField( inTIFF,TIFFTAG_IMAGELENGTH, &ny );
  TIFFGetField( inTIFF,TIFFTAG_IMAGEWIDTH, &nx );
  printf( "ny = %d, nx = %d\n", ny, nx );
  if ( new_bitstore_rgbImage(rgbImage, nx, ny) == -1 ){
    (void) fprintf( stderr, "ERROR : Couldn't allocate memory for rgbImage.\n" );
    return (-1);
  }
    
  // read the image content scanline by scanline into the rgbImage
  MALLOC( lineBuffer, unsigned char, rgbImage->nx * spp, return (-1) ); /// --????--- error handling OK ---????--
  for ( j=0; j<rgbImage->ny; j++ ){
    // get a scanline
    if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ){
      (void) fprintf( stderr, "ERROR : Couldn't read scanline %d from %s.\n", j, inTIFFname );
      return (-1);
    }
    // transfer scanline to rgbImage
    c = 0;
    for( i=0; i<rgbImage->nx; i++ ){
      int) lineBuffer[c++];
  }
}
FREE( lineBuffer );
    
return 0;
}
//


//
// write an rgb image to the disk
int bitstore_rgbImage2TIFF(  bitstore_rgbImage_t *rgbImage, char *outTIFFname ){
  //
  // declare local variables
  TIFF *outTIFF;
  int c,j,i;
  unsigned char *lineBuffer;    
  int min, max;
  //
  // check if the rgbImages actually was allocated
  if ( rgbImage->img == NULL ){
    printf( "ERROR: Can't save rgb image to TIFF since its 'img' field isn't allocated!\n" );
    return (-1); 
  }
  //
  // check if range of gray levels fits for storing as 8-bit image
    min = max = rgbImage->i
      }
if ( min<0 || max>255 ){
  printf( "ERROR: Can't save rgbImage: gray levels of rgb channels out of range!\n" );
  return (-1);
 }
//    
// create TIFF file on disk
if ( (outTIFF = TIFFOpen(outTIFFname, "w"))==NULL ){
  printf( "ERROR: Couldn't create '%s.\n'", outTIFFname );
  return (-1);
 }
//
// write a minimal TIFF heather
TIFFSetField( outTIFF, TIFFTAG_IMAGEWIDTH, rgbImage->nx );
TIFFSetField( outTIFF, TIFFTAG_IMAGELENGTH, rgbImage->ny );
TIFFSetField( outTIFF, TIFFTAG_BITSPERSAMPLE, 8 j<rgbImage->ny; j++ ){
  for ( i=0; i<rgbImage->nx; i++ ){
    for ( c=0; c<3; c++ ){
      lineBuffer[3*i+c] = (unsigned char)(rgbImage->img[c][j][i]);
    }
  }
  if (TIFFWriteScanline (outTIFF, lineBuffer, j, 0) == -1) {
    (void) fprintf (stderr, "ERROR : Couldn't write scan line (%d)! (E#%d)\n", j, __LINE__);
    return (-1);
  }   
}
//
// free memory occupied by buffer
FREE( lineBuffer );
//
// close the TIFF to have it on the disk
TI1;
*spp = *bps = -1;
TIFF *inTIFF;
inTIFF = NULL;
//
if( (inTIFF = TIFFOpen( fileName, "r" )) == NULL ){
  rtrnV = -1;
  printf( "ERROR: Can't read TIFF from file named '%s'\n", fileName );
 }
if ( rtrnV == 1 ){
  TIFFGetField( inTIFF, TIFFTAG_SAMPLESPERPIXEL, spp );
  TIFFGetField( inTIFF, TIFFTAG_BITSPERSAMPLE, bps );
  TIFFGetField( inTIFF, TIFFTAG_IMAGEWIDTH, nx );
  TIFFGetField( inTIFF, TIFFTAG_IMAGELENGTH, ny );
  printf( "> samplesPerPixel = %d\n> bitsPerSample = %d\n", *spp, *bps );
  printf( "> nx = %d\n> ny = %d\n", *nx, *ny );
 }
if ( *spp==-1 || *bps==-1 ){
  rtrnV = -1;
  printf( "ERROR: Couldn't read TIFF-tags from  been allocated with suitable size) --> thread safe
int load_glTIFF_glImage( char *inputTIFFname, bitstore_glImage_t *glImage ){
    //
    int rtrnV = 1;
    int i, j, b;
    unsigned char *lineBuffer;
    TIFF *inTIFF;
    inTIFF = NULL;
    int nx, ny;
    short spp, bps;
    //
    if ( glImage->img == NULL ){
        rtrnV = -1;
        printf( "ERROR: Can't load TIFF to empty gl image.\n" );
    }
    if ( rtrnV == 1 ){
        printf( "> reading '%s': status = %d\n", inputTIFFname, rtrnV );
        if ( (inTIFF = TIFFOpen( inputTIFFname, "r" )) == NULL ){
            rtreld( inTIFF, TIFFTAG_IMAGELENGTH, &ny );
    }
    if( rtrnV!=1 || spp!=1 || bps!=8 || glImage->nx!=nx || glImage->ny!=ny ){
        printf( "ERROR: Can't load '%s' because input parameters are inconsistent.\n", inputTIFFname );
        printf( "/tActual parameters: spp=%d, bps=%d, nx=%d, ny%d\n", spp, bps, nx, ny );
        printf( "/tPassed parameters: spp=%d, bps=%d, nx=%d, ny%d\n", 1, 8, glImage->nx, glImage->ny );
        rtrnV = -1;
    }
    //
    // read the image content scanline by scanline into the glImage
    if ( rtrnV == 1 ){
        MALLOC( lineBuffer, unsigned char, glImage->nx, {printf("ERROR: Can't allocate memory for buffer to hold image scan lines.\n"); rtrnV = -1;} );
        if ( rtrnV == 1 ){
            for ( j                }
                // transfer scanline to glImage
                if ( rtrnV == 1 ){
                    for( i=0; i<glImage->nx; i++ ){
                        glImage->img[j][i] = (int) lineBuffer[i];
                        b = glImage->img[j][i];
                    }
                }
            }
            FREE( lineBuffer );
        }
    }
    //
    if ( inTIFF != NULL ){
        TIFFClose( inTIFF );
    }
    //
    // re//
    if ( channel<0 || channel>2 ){
        rtrnV = -1;
        printf( "ERROR: channel = %d out of bounds, can't load it to gl image.\n", channel );
    }
    if (glImage->img == NULL ){
        rtrnV = -1;
        printf( "ERROR: gl image wasn't allocated, can't load channel %d from rgb TIFF to heap.\n", channel );
    }
    if ( (rtrnV == -1) || (inTIFF = TIFFOpen( inputTIFFname, "r" )) == NULL ){
        rtrnV = -1;
        printf( "ERROR: Couldn't read '%s'.\n",  "ERROR: Unsupported image format spp = %d, bps = %d\n", spp, bps );
            rtrnV = -1;
        }
    }
    //
    // read the image content scanline by scanline into the rgbImage
    if ( rtrnV == 1 ){
        MALLOC( lineBuffer, unsigned char, glImage->nx * 3, {rtrnV = -1; printf( "ERROR: Can't allocate line buffer to transfer image content from TIFF to heap.\n");} ); 
        if ( rtrnV == 1){
            //printf( "channel = %d", channel );
            for ( j=0; j<glImage->ny; j++ ){
                // get a scanline
                if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ){    FREE( lineBuffer );
        }
    }    
    
    //
    return rtrnV;
}
//
int load_rgbTIFF_rgbImage(  char *inputTIFFname, bitstore_rgbImage_t *rgbImage ){
    int rtrnV = 1;
    TIFF *inTIFF;
    inTIFF = NULL;
    unsigned char *lineBuffer;
    int i, j;
    int c;
    
    if ( rgbImage->img == NULL ){
        rtrnV = -1;
        printf( "ERROR: rgb image wasn't allocated, can't load rgb TIFF to heap.\n" );
    }
    if ( rtrnV == 1 ){
        i){
                // get a scanline
                if ( TIFFReadScanline( inTIFF, lineBuffer, j, 0 ) == -1 ){
                    (void) fprintf( stderr, "ERROR : Couldn't read scanline %d from %s.\n", j, inputTIFFname );
                    rtrnV = -1;
                } else {
                // transfer scanline to rgbImage
                    for( i=0; i<rgbImage->nx; i++ ){
                        for ( c=0; c<3; c++ ){
                            rgbImage->img[c][j][i] = (int) lineBuffer[(3*i)+c];
                        }//puff = (3*i)+channel;
                    }
                }
            }
            FREE( lineBuffer ); // only free the buffer if it was successfully allocatint copy_glImage( bitstore_glImage_t *glImg_from, bitstore_glImage_t *glImg_to ){
    int rtrnV = 1;
    int i, j;
    
    if ( glImg_from->img==NULL || glImg_to->img==NULL ) {
        rtrnV = -1;
        printf( "ERROR: Cannot copy gl image since the field 'img' of one of the images points to NULL.\n" );
    } else if ( glImg_from->ny!=glImg_to->ny || glImg_from->nx!=glImg_to->nx ) {
        rtrnV = -1;
        printf( "ERROR: Cannot copy gl image since dimensions don't match!\n" );
    }
    
    if ( rtrnV == 1 ) {
        for ( j=0; j<glImg_from->ny; j++ ) {
            for ( i=0; i<glImg_from->nx; i++ ){
                glImg_to->img[j][i] = glImg_from->img[j][i];
            }
        }
    }
    
    return rtrnV;
}/*
// copy one glImage into another one with same size
int copy_glImage( bitstore_glImage_t *glImage, bitstore_glImage_t *glImCpy ){
    int j,i;
    //
    // check if memory was allocated for 'img' field of both images
    if ( glImCpy->img==NULL || glImage->img==NULL ){
        printf( "ERROR: Can't copy gl image since at least one of the 'img' fileds is NULL.\n" ); 
        return (-1);
    }
    //
    // check if both images were allocated with equal dimensions
    if ( glImage->nx!=glImCpy->nx || glImage->ny!=glImCpy->ny ){
        printf( "ERROR: Can't copy images since dimensions of images are not the same.\n" );
        return (-1);
    }
    //
    // copy the gl of each pixel from the image to the copy
    for( j=0; j<glImage->ny; j++ ){
        for( i=0; i<glImage->nx; i++ ){
            glImCpy->img[j][i] = glImage->img[j][i];
        }
    }
    //
    // report success
#ifdef _VERBOSE_OUTPUT
    printf( "copy_glImage(): 1\n" );
#endif
    return 1;
}
// * /
//
// add glImage to channel c to an rgb image, 'c' in {0,1,2}, 'weight' scales the gl value added to the rgb channel, 'yOffset' and 'xOffset' specify the position of the origin of the added gl image in the coordinate system of the rgb imageusing  
int addChannel_rgbImage( bitstore_glImage_t *glImage, bitstore_rgbImage_t *rgbImage, int c, float weight, int yOffset, int xOffset ){
    //
    // declare local variables
    int j, i, yStartOffset, yEndOffset, xStartOffset, xEndOffset;
    //
    // consistency check of input parameters
    // 1. check that gl and rgb image are nonempty
    if ( glImage->img==NULL || rgbImage->img==NULL ){
        printf( "ERROR: Can't add gl as channel to rgb image since at least one of the images isn't allocated.\n" );
        return (-1);
    }
    // 2. check if the dimensions fit
    if ( glImage->ny!=rgbImage->ny || glImage->nx!=rgbImage->nx ){
        printf( "ERROR: Can't add gl as channel to rgb image since image dimensions are not the same:\n" );
        printf( "        glIm: %d x %d\n       rgbIm: %d x %d\n", glImage->nx, glImage->ny, rgbImage->nx, rgbImage->ny );
        return (-1);
    }
    //
    // set index offsets to handle both positive and negative shifts
    yStartOffset = (-yOffset + abs(yOffset))/2;
    yEndOffset = (yOffset + abs(yOffset))/2;
    xStartOffset = (-xOffset + abs(xOffset))/2;
    xEndOffset = (xOffset + abs(xOffset))/2;
    //
    // add the gl image to the specified rgb channel
    for ( j=yStartOffset; j<(rgbImage->ny - yEndOffset); j++ ){
        for( i=xStartOffset; i<(rgbImage->nx - xEndOffset); i++ ){
            rgbImage->img[c][j+yOffset][i+xOffset] += (int)(weight*glImage->img[j][i]);
        }
    }
    //
    // report success
#ifdef _VERBOSE_OUTPUT
    printf( "addChannel_rgbImage(): 1\n" );
#endif
    return 1;
}
//
int set_channel_rgbImage( bitstore_glImage_t *glImage, bitstore_rgbImage_t *rgbImage, int c, float weight ){
    //
    // declare local variables
    int j, i;
    //
    // consistency check of input parameters
    // 1. check that gl and rgb image are nonempty
    if ( glImage->img==NULL || rgbImage->img==NULL ){
        printf( "ERROR: Can't add gl as channel to rgb image since at least one of the images isn't allocated.\n" );
        return (-1);
    }
    // 2. check if the dimensions fit
    if ( glImage->ny!=rgbImage->ny || glImage->nx!=rgbImage->nx ){
        printf( "ERROR: Can't add gl as channel to rgb image since image dimensions are not the same:\n" );
        printf( "        glIm: %d x %d\n       rgbIm: %d x %d\n", glImage->nx, glImage->ny, rgbImage->nx, rgbImage->ny );
        return (-1);
    }
    //
    // set gl of every pixel in the specified rgb channel according to the gl input
    for ( j=0; j<rgbImage->ny; j++ ){
        for( i=0; i<rgbImage->nx; i++ ){
            rgbImage->img[c][j][i] = (int)(weight*glImage->img[j][i]);
        }
    }
    //
    // report success
#ifdef _VERBOSE_OUTPUT
    printf( "set_channel_rgbImage(): 1\n" );
#endif
    return 1;
    
}


//
// convert a rgb image to a gray level image.
int convert_rgb2glImage( bitstore_rgbImage_t *rgbImage, bitstore_glImage_t *glImage) {
    int i, j;
    int rtrnV = 0;
    
    if ((rgbImage == NULL) || (glImage == NULL)) {
        fprintf(stderr, "Error: Must give two initialized images!\n");        
        rtrnV = 1;
        return rtrnV;
    }
    
    // check if image dimensions match
    if ((rgbImage->nx != glImage->nx) || (rgbImage->ny != glImage->ny)) {
        fprintf(stderr, "Error: Dimensions of images do not match!\n");
        rtrnV = 1;
        return rtrnV;
    }
    
    for (j = 0; j < glImage->ny; j++) {
        for (i = 0; i < glImage->nx; i++) {
            glImage->img[j][i] =  (int)(0.30f * rgbImage->img[0][j][i]);
            glImage->img[j][i] += (int)(0.59f * rgbImage->img[1][j][i]);
            glImage->img[j][i] += (int)(0.11f * rgbImage->img[2][j][i]);
        }
    }
    return rtrnV;
}
//
// extracts a single channel of a rgb image as gray level image
int extractChannel_rgbImage( bitstore_rgbImage_t *rgbImage, bitstore_glImage_t *glImage, int channel) {
    int i, j;
    int rtrnV = 0;
    
    if ((rgbImage == NULL) || (glImage == NULL)) {
        fprintf(stderr, "Error: Must give two initialized images!\n");        
        rtrnV = 1;
        return rtrnV;
    }
    
    // check if image dimensions match
    if ((rgbImage->nx != glImage->nx) || (rgbImage->ny != glImage->ny)) {
        fprintf(stderr, "Error: Dimensions of images do not match!\n");
        rtrnV = 1;
        return rtrnV;
    }

    // check if channel number is 0..2
    if ((channel < 0) || (channel > 2)) {
        fprintf(stderr, "Error: Invalid channel numner. Must be 0, 1 or 2.\n");
        rtrnV = 1;
        return rtrnV;
    }

    for (j = 0; j < rgbImage->ny; j++) {
        for (i = 0; i < rgbImage->nx; i++) {
            glImage->img[j][i] =  rgbImage->img[channel][j][i];
        }
    }
    return rtrnV;
}
//
//
// copy a subset from a glImage into a smaller one
int copy_glImageROI( bitstore_glImage_t *glImage, bitstore_glImage_t *crpGlImage, int x_offset, int y_offset ){
    int rtrnV = 1;
    int i, j;
    int i_max, j_max;
  
    if( glImage->img==NULL || crpGlImage->img==NULL ){
        rtrnV = -1;
        printf( "ERROR: Can't copy ROI from gl image, because image(s) were not allocated properly.\n" );
    }
    
    if( rtrnV==1 ){
#ifdef _VERBOSE_OUTPUT
        printf( "copying ROI from gl image located between ul: (%d,%d)=%d and lr: (%d,%d)=%d\n", x_offset, y_offset, glImage->img[y_offset][x_offset], x_offset+crpGlImage->nx-1, y_offset+crpGlImage->ny-1, glImage->img[y_offset+crpGlImage->ny-1][x_offset+crpGlImage->nx-1] );
#endif
        // copy 
        i_max = min(crpGlImage->nx,glImage->nx-x_offset);
        j_max = min(crpGlImage->ny,glImage->ny-y_offset);
        //printf( "%d < i < %d, %d < j < %d\n", 0, i_max, 0, j_max );
        for ( j=0; j<j_max; j++ ){
            for( i=0; i<i_max; i++ ){
                crpGlImage->img[j][i] = glImage->img[j+y_offset][i+x_offset];
            }
        }
        
    }    
    
    return rtrnV;
}
//
//
// just the maximum of integers
int max( int a, int b ){
    int rtrnV;
    if ( a > b ){
        rtrnV = a;
    }else{
        rtrnV = b;
    }
    return rtrnV;
}
// just the minimum function
int min( int a, int b ){
    if ( a<b ) return a;
    else return b;
}
//

 */
