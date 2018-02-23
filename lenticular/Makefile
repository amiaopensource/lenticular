CC = gcc
CFLAGS = -Wall -std=c99

OBJ = imageHandling.o imageProcessing.o
BIN: lenticular inStudy frameCropper

lenticular: lenticular.o $(OBJ)
	$(CC) $(CFLAGS) -ltiff lenticular.o $(OBJ) -o lenticular

inStudy: interpolationStudy.o $(OBJ)
	$(CC) $(CFLAGS) -ltiff interpolationStudy.o $(OBJ) -o inStudy

frameCropper: frameCropper.o $(OBJ)
	$(CC) $(CFLAGS) -ltiff frameCropper.o $(OBJ) -o frameCropper

obj: $(OBJ)
	$(CC) $(OBJ) -c $(OBJ)

.PHONY: clean
clean:
	rm *.o
