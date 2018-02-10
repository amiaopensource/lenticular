# Makefile
#
# this is part of:
#
# doLCE (do Lenticular film Color rEconstruction)
# Copyright (c) 2012 Joakim Reuteler
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3 as published
# by the Free Software Foundation.
#
# HISTORY
#   2018-02-05 purge code



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

.PHONY: clean
clean:
	rm *.o
