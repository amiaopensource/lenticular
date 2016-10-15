//
// doLCE - do Lenticular film Color rEconstruction -
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



// imageHandling.h 
// - definitions of structures to hold gray level and rgb images
// - methods to allocate and free memory
// - reading and writing TIFF images

/*
 ideas pursued in this code:
 0. No additional comments, aim is to have the code itself understandable 
 1. new_...() only allocates memory, values are not set
 2. new_..() operates only if data containing arrays point to NULL
 3. delete_...() releases memory and sets pointers on data containing fields to NULL
 4. methods do not allocate or release memory (reentrance safe?)
 5. pixel coordinates are ordered as in mathematics: x,y
 6. using a memState_objectXY varibale to keep track of allocation and freeing memory in the main() is advised!
 7. return value 0 <-> OK, other <-> ERROR
 8. methods only report in case of problems, they are quiet in case of success (main() shall decide when to report success) 
 
The code is based on the one used for the "Monolith 2" project conducted IML, University of Basel. The code here is an extensively reorganized version of that code.
*/

// imageHandling.h
// - declaration of types to hold images
// - ceclaration of methods for handling of such images
// - declaration of methods for reading from and writing to TIFF
#ifndef doLCE_imageHandling_h
#define doLCE_imageHandling_h



// *** imageTypes.h ***
// **************************************

typedef struct{
  int width, height;    
  int **img;
  int memState;
} glImage_t;

int new_glImage( glImage_t *glImage, int width, int height );

int delete_glImage( glImage_t *glImage );

typedef struct{
  int width, height;
  int ***img;
  int memState;
} rgbImage_t;

int new_rgbImage( rgbImage_t *rgbImage, int width, int height );

int delete_rgbImage( rgbImage_t *rgbImage );


// *** handlingTIFF.h ***
// **********************
int check_TIFF( char *TIFFname, short *spp, short *bps, int *width, int *height );

int read_TIFF_glImage( char *fromTIFFname, glImage_t *glImage );

int read_16bitTIFF_glImage( char *fromTIFFname, glImage_t *glImage );

int read_3x8bitTIFF_rgbImage( char *fromTIFFname, rgbImage_t *rgbImage ); 

int read_3x16bitTIFF_rgbImage( char *fromTIFFname, rgbImage_t *rgbImage ); 

int write_16bitTIFF_glImage( glImage_t *glImage, char *toTIFFname ); 

int write_3x8bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFname );

int writeROI_3x8bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFname, int roiXpos, int roiYpos, int roiWidth, int roiHeight );

int write_3x16bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFName ); // currently implementing...
 


// *** geometryTypes.h ***
// **********************************************
typedef struct {
  int x, y;
} point_t;

// to do: line, rectangle 


typedef struct {
  int length;
  int *list; //point_t coo[]; to be changed from int array to point_t array once I know how that works...
  int memState;
} positionList_t;

int new_positionList( positionList_t *positions, int length );

int delete_positionList( positionList_t *positions );

/*
int pick_NonzeroPositions_fromProfile( glprofile_t *profile, positionList_t *positions );
*/


// *** grayLevelAnalysis.h ***
// ****************************************
int minMax_glImage( glImage_t *glImage, int *minGl, int *maxGl ); // should be called get_range_glImage()

int get_range_rgbImage( rgbImage_t *rgbImage, int minValue[3], int maxValue[3] );

typedef struct {
    int minGl;
    int maxGl;
    unsigned long *freq; 
  int memState;
} glHistogram_t;

int new_glHistogram( glHistogram_t *glHist, int minGl, int maxGl );

int delete_glHistogram( glHistogram_t *glHist );

int get_glHistogram( glImage_t *glImage, glHistogram_t *glHist ); 

int get_glHistograms_fourShot( glImage_t *glImage, glHistogram_t glHists[4] );

int write_glHistogram( glHistogram_t *glHist, char *toFileName );

int write_glHistograms_fourShot( glHistogram_t glHists[4], char *toFileName );

typedef struct {
  int direction; // 0 <-> along x, 1 <-> along y 
  point_t p; 
  int length;
  int thickness; // number of pixel summed over in perpendicular direction
  long *glSum; // need long to avoid overflow!
  int memState; // 0 <-> not allocated, -1 <-> unknown (error)
} glProfile_t;

int new_glProfile( glProfile_t *glProfile, int length );

int delete_glProfile( glProfile_t *glProfile );

int get_glProfile( glImage_t *glImage, glProfile_t *glProfile, point_t p, int direction, int thickness );

int write_glProfile( glProfile_t *glProfile, char *tofileName );

int copy_glProfile( glProfile_t *glProfileORI, glProfile_t *glProfileCPY );

int smooth_glProfile( glProfile_t *glProfile );

//int erode_glProfile( glProfile_t *glProfile );

int extract_localMinima_glProfile( glProfile_t *profile ); 

int extract_wells_glProfile( glProfile_t *profile );

int get_meanPeakSpacing( glProfile_t *profile, float *peakSpacing );

int get_peakSpacingHistogram( glProfile_t *profile, int peakSpacingHist[40], int *nPeaks );

int get_rasterSpacing( int spacingHist[40], int *rasterSpacing );

int regularize_peakRaster( glProfile_t *glProfile, int rasterSpacing, int *nPeaks ); // rebame Raster to Profile, dito below!!!!!

int relax_regPeakRaster( glProfile_t *glProfile, glProfile_t *regPeakProfile );

int count_peaks_glProfile( glProfile_t *profile, int *nPeaks );

int get_peakPositions( glProfile_t *profile, positionList_t *peakPos );

int correct_oddPeakPositions( positionList_t *peakPos );


// *** reconstructing the colors ***
// *********************************

int reconstruct_colorFrame( glImage_t *glScan, positionList_t *rasterPos, rgbImage_t *rgbFrame );

int reconstructInterpolate_colorFrame( glImage_t *glScan, positionList_t *rasterPos, rgbImage_t *rgbFrame );

int pickColor_interactively( glImage_t *glScan, positionList_t *rasterPos );

// *** drawingInImages.h ***
// **************************

int draw_verticalLines_rgbImage( rgbImage_t *rgbImage, positionList_t *positions, int color[3] ); 

int draw_raster_rgbImage( rgbImage_t *rgbImage, glProfile_t *profile, positionList_t *positions );

// to do: lines and rectangles


#endif


// 
/* BEGIN not yet revised code
int write_glImage_TIFF( glImage_t *glImage, char *TIFFname );

int read_TIFF_rgbImage( char *TIFFname, rgbImage_t *rgbImage );

int write_rgbImage_TIFF( rgbImage_t *rgbImage, char *TIFFname );
// END not yet revised code 
*/

/// WHAT TO DO WITH THAT BELOW??  DON'T YET THROW!

// all stuff below should better go elsewhere I think (JR Nov 14 2012)
/*
//
// load an 8-bit gl TIFF to a gl image which was allocated with appropriate dimensions
int load_glTIFF_glImage( char *inputTIFFname, bitstore_glImage_t *glImage );
//
// load one channel of an 8-bit rgb TIFF to a gl image which was allocated with appropriate dimensions
int loadChannel_rgbTIFF_glImage( char *inputTIFFname, bitstore_glImage_t *glImage, int channel );
//
int load_rgbTIFF_rgbImage(  char *inputTIFFname, bitstore_rgbImage_t *rgbImage );
//


// *******************************************
// *** basic handling of gl and rgb images ***
// *******************************************
//
// copy one glImage into another one with same size
int copy_glImage( bitstore_glImage_t *glImg_from, bitstore_glImage_t *glImg_to );
//
// add glImage weighted to channel c E {0,1,2} to an rgb image
int addChannel_rgbImage( bitstore_glImage_t *glImage, bitstore_rgbImage_t *rgbImage, int c, float weight, int yOffset, int xOffset );
//
int set_channel_rgbImage( bitstore_glImage_t *glImage, bitstore_rgbImage_t *rgbImage, int c, float weight );
//
// convert a rgb image to a gray level image.
int convert_rgb2glImage( bitstore_rgbImage_t *rgbImage, bitstore_glImage_t *glImage);
//
// extracts a single channel of a rgb image as gray level image
int extractChannel_rgbImage( bitstore_rgbImage_t *rgbImage, bitstore_glImage_t *glImage, int channel);
//
// copy a subset from a glImage into a smaller one
int copy_glImageROI( bitstore_glImage_t *glImage, bitstore_glImage_t *crpGlImage, int x_offset, int y_offset );
//
// just the maximum of integers
int max( int a, int b );
//
// just the minimum of integers
int min( int a, int b );
//
 */

