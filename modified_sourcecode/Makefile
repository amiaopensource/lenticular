CC=gcc

CFLAGS=-Wall

doLCE: lenticularFilmColorReconstruction.o imageHandling.o imageProcessing.o
	cc -ltiff lenticularFilmColorReconstruction.o imageHandling.o imageProcessing.o -o doLCE

inStudy: interpolationStudy.o imageHandling.o imageProcessing.o
	cc -ltiff interpolationStudy.o imageHandling.o imageProcessing.o -o inStudy

crpFrames: frameCropper.o imageHandling.o imageProcessing.o
	cc -ltiff frameCropper.o imageHandling.o imageProcessing.o -o crpFrames

imageHandling.o: imageHandling.c imageHandling.h
	cc -c imageHandling.c

imageProcessing.o: imageProcessing.c imageProcessing.h
	cc -c imageProcessing.c

clean:
	rm imageHandling.o lenticularFilmColorReconstruction.o imageProcessing.o doLCE
	rm frameCropper.o crpFrames
