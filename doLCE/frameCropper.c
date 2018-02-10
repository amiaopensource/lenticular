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



#include "imageHandling.h"
#include "imageProcessing.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main( int argc, char *argv[] ) {
  // processing control
  int status = 0;
  
  // constants  
  char greetingText[128] = "";
  sprintf( greetingText, "*\n**\n*** frameCropper - crop center part of rgb frames ***\n**\n*   [%s ~ %s]\n\n", __DATE__, __TIME__);
  char copyrightText[200] = "This program is free software under the terms of the GNU General Public License version 3.\nSee <http://www.gnu.org/licenses/>.\n\n";
  char helpText[] = "frameCropper 'width' 'height' 'inputBaseName' 'startNo' 'endNo' 'outputDir'\n";
  
  char inputImageName[128] = "";
  char inputBaseName[128] =  "";
  char startNoStr[16] = "";
  int startNo;
  char endNoStr[16] = "";
  int endNo;
  char outputDirName[128] = "";
  char outputImageName[132] = "";
  
  int frameNo;
  char frameNoFormat[8] = "";
  char frameNoStr[16] = "";
  
  // variables
  int argNo, optNo;
  short spp, bps;
  int width, height;
  int crpWidth, crpHeight;
  int xOffset, yOffset;
  int j, i;
  int channel;  

  // data
  rgbImage_t inImg;
  inImg.img = NULL;
  inImg.memState = 0;
  rgbImage_t outImg;
  outImg.img = NULL;
  outImg.memState = 0;
  
  
  // Plan:
  // - read an rgb image, copy only central part with a size of 'crpWidth' x 'crpHeight' and write it to the disk


  printf( "%s", greetingText );
  printf( "%s", copyrightText );
  
  if ( argc < 7 ) {
    status = -1;
    printf( "Too few arguments.\n" );
    printf( "\n" );
    printf( "%s", helpText );
  }
  else {
    argNo = 1;
    printf( "argNo = %d\n", argNo );
    crpWidth = atoi( argv[argNo] );
    crpHeight = atoi( argv[++argNo] );
    printf( "crpWidth = %d, crpHeight = %d\n", crpWidth, crpHeight );
    strcpy( inputBaseName, argv[++argNo] );
    //printf( "inputBaseName = %s\n", inputBaseName );
    strcpy( startNoStr, argv[++argNo] );
    //printf( "startNoStr = %s\n", startNoStr );
    startNo = atoi( argv[argNo] );
    //printf( "startNo = %d\n", startNo );
    strcpy( endNoStr, argv[++argNo] );
    //printf( "endNoStr = %s\n", endNoStr );
    endNo = atoi( argv[argNo] );
    //printf( "endNo = %d\n", endNo );
    strcpy(  outputDirName, argv[++argNo] );
    //printf( "outputDirName = %s\n", outputDirName );
        
    printf( "image sequence to be processed : '%s%s.tif' to '%s%s.tif'\n", inputBaseName, startNoStr, inputBaseName, endNoStr );
    printf( "cropped frames will be stored as '%s/crp_%s%s.tif' etc.\n", outputDirName, inputBaseName, startNoStr ); 
        
    //status = -1;
  }
        
  if ( status == 0 ) {
    printf( "\nlog of call : '" );
    for ( argNo=0; argNo<argc; argNo++ ) {
      printf( "%s ", argv[argNo] );
    }
    printf( "'\n" );

    if ( strlen(startNoStr) == strlen(endNoStr) ) {
      if ( strlen(startNoStr) == 1 ) {
	sprintf( frameNoFormat, "%%d" );
	//printf( "frameNoFormat = %s\n", frameNoFormat );
      }
      else {
	sprintf( frameNoFormat, "%%0%dd", (int)strlen(startNoStr) );
	//printf( "frameNoFormat = %s\n", frameNoFormat );
      }    
    }
    else {
      if ( strlen(startNoStr)>1 && startNoStr[0]==0 ){
	status = -1;
	printf( "ERROR: Cannot handle this kind of numbering\n" );
	printf( "startNoStr[0] == 0\n" );
	printf( "startNoStr == %s\n", startNoStr );
      }
      else {
	sprintf( frameNoFormat, "%%d", strlen(startNoStr) );
	//printf( "frameNoFormat = %s\n", frameNoFormat );
      }
    }
  }
      
  if ( status == 0 ) {
    printf( "\n-------------------loop over frames----------------------\n\n" );
    //}
    for ( frameNo=startNo; frameNo<=endNo && status==0; frameNo++ ) {
      sprintf( frameNoStr, frameNoFormat, frameNo );
      //printf( "frameNoStr = %s\n", frameNoStr );  
      sprintf( inputImageName, "%s%s.tif", inputBaseName, frameNoStr );
      sprintf( outputImageName, "%s/crp_%s%s.tif", outputDirName, inputBaseName, frameNoStr );
      printf( "***************************************************************************\n" );
      printf( "*** processing frame %d/%d ('%s')\n", frameNo-startNo+1, endNo-startNo+1, inputImageName );
        
      if ( status == 0 ) {
	printf( "Checking TIFF...\n" );
	status = check_TIFF( inputImageName, &spp, &bps, &width, &height );
	if ( status == 0 ) {
	  printf( "> % 5d samples per pixel \n> % 5d bits per sample\n", spp, bps );  // change into "normal" format: blabla = value ?
	  printf( "> % 5d pixels wide\n> % 5d pixels high\n", width, height );
	  if ( inImg.memState==1 && (inImg.width!=width || inImg.height!=height) ) {
	    delete_rgbImage( &inImg );
	    printf( "inImg.memState = %d\n", inImg.memState );
	  }
	  if ( inImg.memState == 0 ) {
	    status = new_rgbImage( &inImg, width, height );//new_glImage( &glScan, width, height );
	    printf( "### allocated memory: 'inImg.memState' = %d\n", inImg.memState );        
	  }
	  if ( inImg.memState == 1 ) {
	    printf( "Loading TIFF...\n" );
	    status = read_3x8bitTIFF_rgbImage( inputImageName, &inImg );
	    //status = read_16bitTIFF_glImage( inputImageName, &glScan );
	    if ( status == 0 ) {
	      printf( "> OK.\n" );
	    }
	  }
	  else {
	    status = -1;
	    printf( "ERROR: No memory for loading TIFF available.\n" );
	  }
	}
      }
      
      if ( status == 0 ) {
	xOffset = (width - crpWidth)/2;
	yOffset = (height - crpHeight)/2;
	printf( "xOffset = %d, yOffset = %d", xOffset, yOffset ); 
	if ( xOffset>=0 && yOffset>=0 ) {
	  printf( "Writing result to disk...\n" );
	  status = writeROI_3x8bitTIFF_rgbImage( &inImg, outputImageName, xOffset, yOffset, crpWidth, crpHeight );
	  if ( status == 0 ) {
	    printf( "Stored result to '%s'.\n", outputImageName );
	  }
	}
	else {
	  printf( "Skipping image because it is too small.\n" );
	}
      }
      
      printf( "*** done processing frame No %d\n", frameNo );
      printf( "***************************************************************************\n\n" );
    }
    if ( status == 0 ) {
      printf( "\n---------------------end loop over frames------------------------\n" );
    }
  }
    
  if ( inImg.memState != 0 ) {
    printf( "Cleaning up heap...\n" );
    delete_rgbImage( &inImg );
    printf( "### freed memory 'inImg.memState = %d'\n", inImg.memState ); 
  }
/*
  if ( outImg.memState != 0 ) {
    delete_rgbImage( &outImg );
    printf( "### freed memory 'outImg.memState = %d'\n", inImg.memState ); 
  }
*/

  printf( "\nStatus at end : %d\n", status );
  return status;
}
