CC = gcc
CFLAGS = -Wall -std=c99

OBJ = imageHandling.o imageProcessing.o
BIN: lenticular framecropper interpolationstudy

lenticular: lenticular.o $(OBJ)
	$(CC) $(CFLAGS) -ltiff lenticular.o $(OBJ) -o lenticular

framecropper: frameCropper.o $(OBJ)
	$(CC) $(CFLAGS) -ltiff frameCropper.o $(OBJ) -o framecropper

interpolationstudy: interpolationStudy.o $(OBJ)
	$(CC) $(CFLAGS) -ltiff interpolationStudy.o $(OBJ) -o interpolationstudy

obj: $(OBJ)
	$(CC) $(OBJ) -c $(OBJ)

.PHONY: clean
clean:
	rm *.o
