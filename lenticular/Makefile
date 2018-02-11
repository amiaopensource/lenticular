CC = gcc
CFLAGS = -Wall -std=c99

all: doLCE inStudy frameCropper

doLCE: lenticularFilmColorReconstruction.o imageHandling.o imageProcessing.o
	$(CC) $(CFLAGS) -ltiff lenticularFilmColorReconstruction.o imageHandling.o imageProcessing.o -o doLCE

inStudy: interpolationStudy.o imageHandling.o imageProcessing.o
	$(CC) $(CFLAGS) -ltiff interpolationStudy.o imageHandling.o imageProcessing.o -o inStudy

frameCropper: frameCropper.o imageHandling.o imageProcessing.o
	$(CC) $(CFLAGS) -ltiff frameCropper.o imageHandling.o imageProcessing.o -o frameCropper

imageHandling.o: imageHandling.c imageHandling.h
	$(CC) $(CFLAGS) -c imageHandling.c

imageProcessing.o: imageProcessing.c imageProcessing.h
	$(CC) $(CFLAGS) -c imageProcessing.c

.PHONY: clean
clean:
	rm *.o
