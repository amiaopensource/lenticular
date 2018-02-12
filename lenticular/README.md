# Digital Restoration of Lenticular Colours from Greyscale Digitisations

## ☛ **This is a work in progress** ☚

### Contributions are very welcome!

Please:

- keep the [issue tracker](http://github.com/amiaopensource/lenticular/issues) limited to bug reports and feature requests;
- use the [pull requests](http://github.com/amiaopensource/lenticular/pulls) to submit patches.

Thank you!

### Short-term goal

- Any idea for a better name than `doLCE` for the modified version?
- I was told a logo would be important.
- Understand and document the parameters.
- Upload test files on GitHub.
- Use the standard `(x,y)` order of coordinates, instead of the inverted one `(y,x)` used by Monolith 2.
- Allow full path for both the input and the output folder. In particular, the output folder has not to be nested inside the input folder, but can be e.g. on another hard-disk drive.
- Allow the file extensions `.tiff`, `.TIF` and `.TIFF` as well.
- Allow more TIFF flavours to be processed, _in primis_ `rgb48le`, because it seems that currently only `gray16le` is supported.
- Determine automatically the regex for parsing the greyscale frames of the input folder.
- The number of the first and of the last frame to process is needed only for testing a subset of the frames in the folder. If these parameters are not provided, the parse all the files in the folder.

### Mid-term goal

- Could this be a project for the next AMIA Hackday?
- Verify that TIFF is really the best choice as processing format.
- Process any image format supported by FFmpeg.

### Long-term goals of the project

- **CLI:** A command-line programme with a library that can be used e.g. from FFmpeg (`ffmpeg` and `ffplay`) or `mpv` as well.
- **GUI:** A stand-alone programme, directly inspired by [AEO-Light](https://usc-imi.github.io/aeo-light/), that allows to generate the colours of digitised lenticular movie films, like _Kodacolor_ and _Agfacolor_.

---

## An alpha release is available for testing purpose

### How to compile the source

1. download the `lenticular` folder
1. open the terminal
1. `cd` into the folder
1. run `make && make clean`

### How to run the main tool

1. open the terminal
1. `cd` into the folder
1. run `./doLCE`

### Homebrew and Linuxbrew

Alternatively, you can install the modified `doLCE` tool by using Homebrew or Linuxbrew: run the command:

```
brew install amiaopensource/amiaos/lenticular
```

Once installed, run the main tool by the command:

```
doLCE
```

Brew installation and `doLCE` have been successfully tested on the following operating systems:

- **Linux:** Ubuntu 16.04.3 LTS and 14.04.5 LTS; Slackware 14.2; Debian 7.11
- **Mac:** macOS 10.13.3 and 10.12.6, OS X 10.11.6 and 10.10.5; 
- **Windows:** Subsystem for Linux (aka Ubuntu 14.04.4 LTS)

### Parameters

```
modified doLCE 2018-02-12 alpha

doLCE [-help] [-highRes] [-profileRelThickness (float)]
  [-profileRelPosY (float)] [-relaxRaster]
  [-rasterSpacing (float)] [-troubleshoot]
  'inputDir' 'inputBaseName' 'startNo' 'endNo' 'outputDir'
```

- **inputDir:** path to the input directory, e.g. `~/TEST/SOURCE_FILES/`
- **inputBaseName:** input base filename without the numbering, e.g. `greyscale_`
- **startNo:** number of the first file, e.g. `0001`
- **endNo:** number of the last file, e.g. `0057`
- **outputDir:** output directory, e.g. `NEW_FILES`

Please note that, like in the original code, as well as in the current version:

- the file extension must be `.tif` and the extension `.tiff` is not allowed
- in the example above the first input file is `~/TEST/SOURCE_FILES/greyscale_0001.tif`
- the output folder must be nested inside the input folder
- only the name of the output folder must be given
- in the example above the first output file is `~/TEST/SOURCE_FILES/NEW_FILES/rgb_greyscale_0001.tif`

### Additional tools

The original source code contains the following two additional tools. You can install one or both of them via Homebrew or Linuxbrew as well.

**Note:** We haven’t started to explore the additional tools so far.

#### inStudy

To install `inStudy` via Homebrew or Linuxbrew, run the command:

```
brew install amiaopensource/amiaos/lenticular --with-inStudy
```

Once installed, run the main tool by the command:

```
inStudy
```

which should give:

```
modified inStudy 2018-02-12 alpha
  RGB image pixel column interpolation study

Too few arguments.

inStudy [--help] 'inputFileName'

Status at end : -1
```

#### frameCropper

To install `frameCropper` via Homebrew or Linuxbrew, run the command:

```
brew install amiaopensource/amiaos/lenticular --with-frameCropper
```

Once installed, run the main tool by the command:

```
frameCropper
```

which should give:


```
modified frameCropper 2018-02-12 alpha
  crop center part of RGB frames

Too few arguments.

frameCropper 'width' 'height' 'inputBaseName' 'startNo' 'endNo' 'outputDir'

Status at end : -1
```

---

## Licence and Disclaimer

The current maintainer is [Reto Kromer](https://github.com/retokromer).

This work is released under a [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) Licence.

This work is provided «as is» without warranty or support of any kind.
