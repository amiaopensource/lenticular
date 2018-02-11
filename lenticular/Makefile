CC = gcc
CFLAGS = -Wall -std=c99

TOOLS = imageHandling.o imageProcessing.o

all: doLCE inStudy frameCropper

doLCE: lenticularFilmColorReconstruction.o $(TOOLS)
	$(CC) $(CFLAGS) -ltiff lenticularFilmColorReconstruction.o $(TOOLS) -o doLCE

inStudy: interpolationStudy.o $(TOOLS)
	$(CC) $(CFLAGS) -ltiff interpolationStudy.o $(TOOLS) -o inStudy

frameCropper: frameCropper.o $(TOOLS)
	$(CC) $(CFLAGS) -ltiff frameCropper.o $(TOOLS) -o frameCropper

tools: $(TOOLS)
	$(CC) $(FLAGS) -c tools $(TOOLS)

.PHONY: clean
clean:
	rm *.o
