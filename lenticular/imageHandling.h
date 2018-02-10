/******************************************************************************
 *
 * imageHandling.h
 * - define structures to hold gray level and RGB images
 * - memory handling
 * - read and write TIFF images
 *
 * this is part of:
 *
 * doLCE (do Lenticular film Color rEconstruction)
 * Copyright (c) 2012 Joakim Reuteler
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation.
 *
 * HISTORY
 *   2018-02-05 purge code
 *
 ******************************************************************************/



/*
 ideas pursued in this code:

 0. No additional comments, aim is to have the code itself understandable
 1. new_...() only allocates memory, values are not set
 2. new_..() operates only if data containing arrays point to NULL
 3. delete_...() releases memory and sets pointers on data containing fields
    to NULL
 4. methods do not allocate or release memory (reentrance safe?)
 5. pixel coordinates are ordered as in mathematics: x,y
 6. using a memState_objectXY varibale to keep track of allocation and
    freeing memory in the main() is advised!
 7. return value 0 <-> OK, other <-> ERROR
 8. methods only report in case of problems, they are quiet in case of
    success (main() shall decide when to report success)

 The code is based on the one used for the "Monolith 2" project conducted
 IML, University of Basel. The code here is an extensively reorganized
 version of that code.
*/

#ifndef doLCE_imageHandling_h
#define doLCE_imageHandling_h



// imageTypes.h

typedef struct {
  int width, height;
  int **img;
  int memState;
} glImage_t;

int new_glImage( glImage_t *glImage, int width, int height );
int delete_glImage( glImage_t *glImage );

typedef struct {
  int width, height;
  int ***img;
  int memState;
} rgbImage_t;

int new_rgbImage( rgbImage_t *rgbImage, int width, int height );
int delete_rgbImage( rgbImage_t *rgbImage );



// handlingTIFF.h

int check_TIFF( char *TIFFname, short *spp, short *bps, int *width, int *height );
int read_TIFF_glImage( char *fromTIFFname, glImage_t *glImage );
int read_16bitTIFF_glImage( char *fromTIFFname, glImage_t *glImage );
int read_3x8bitTIFF_rgbImage( char *fromTIFFname, rgbImage_t *rgbImage );
int read_3x16bitTIFF_rgbImage( char *fromTIFFname, rgbImage_t *rgbImage );
int write_16bitTIFF_glImage( glImage_t *glImage, char *toTIFFname );
int write_3x8bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFname );
int writeROI_3x8bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFname, int roiXpos, int roiYpos, int roiWidth, int roiHeight );
int write_3x16bitTIFF_rgbImage( rgbImage_t *rgbImage, char *toTIFFName );



// geometryTypes.h

typedef struct {
  int x, y;
} point_t;



// to do: line, rectangle

typedef struct {
  int length;
  int *list;
  int memState;
} positionList_t;

int new_positionList( positionList_t *positions, int length );
int delete_positionList( positionList_t *positions );



// grayLevelAnalysis.h

int get_range_glImage( glImage_t *glImage, int *minGl, int *maxGl );
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
  int direction;  // 0 <-> along x, 1 <-> along y
  point_t p;
  int length;
  int thickness;  // number of pixel summed over in perpendicular direction
  long *glSum;    // need long to avoid overflow!
  int memState;   // 0 <-> not allocated, -1 <-> unknown (error)
} glProfile_t;

int new_glProfile( glProfile_t *glProfile, int length );
int delete_glProfile( glProfile_t *glProfile );
int get_glProfile( glImage_t *glImage, glProfile_t *glProfile, point_t p, int direction, int thickness );
int write_glProfile( glProfile_t *glProfile, char *tofileName );
int copy_glProfile( glProfile_t *glProfileORI, glProfile_t *glProfileCPY );
int smooth_glProfile( glProfile_t *glProfile );
int extract_localMinima_glProfile( glProfile_t *profile );
int extract_wells_glProfile( glProfile_t *profile );
int get_meanPeakSpacing( glProfile_t *profile, float *peakSpacing );
int get_peakSpacingHistogram( glProfile_t *profile, int peakSpacingHist[40], int *nPeaks );
int get_rasterSpacing( int spacingHist[40], int *rasterSpacing );
int regularize_peakRaster( glProfile_t *glProfile, int rasterSpacing, int *nPeaks );
int relax_regPeakRaster( glProfile_t *glProfile, glProfile_t *regPeakProfile );
int count_peaks_glProfile( glProfile_t *profile, int *nPeaks );
int get_peakPositions( glProfile_t *profile, positionList_t *peakPos );
int correct_oddPeakPositions( positionList_t *peakPos );



// reconstructing the colours

int reconstruct_colorFrame( glImage_t *glScan, positionList_t *rasterPos, rgbImage_t *rgbFrame );
int reconstructInterpolate_colorFrame( glImage_t *glScan, positionList_t *rasterPos, rgbImage_t *rgbFrame );
int pickColor_interactively( glImage_t *glScan, positionList_t *rasterPos );



// drawingInImages.h

int draw_verticalLines_rgbImage( rgbImage_t *rgbImage, positionList_t *positions, int color[3] );
int draw_raster_rgbImage( rgbImage_t *rgbImage, glProfile_t *profile, positionList_t *positions );

#endif
