CC = gcc
CFLAGS = -Wall -std=c99

OBJ = imageHandling.o imageProcessing.o
BIN: doLCE inStudy frameCropper

doLCE: lenticularFilmColorReconstruction.o $(OBJ)
	$(CC) $(CFLAGS) -ltiff lenticularFilmColorReconstruction.o $(OBJ) -o doLCE

inStudy: interpolationStudy.o $(OBJ)
	$(CC) $(CFLAGS) -ltiff interpolationStudy.o $(OBJ) -o inStudy

frameCropper: frameCropper.o $(OBJ)
	$(CC) $(CFLAGS) -ltiff frameCropper.o $(OBJ) -o frameCropper

obj: $(OBJ)
	$(CC) $(OBJ) -c tools $(OBJ)

.PHONY: clean
clean:
	rm *.o
