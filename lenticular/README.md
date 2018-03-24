# Digital Restoration of Lenticular Colours from Greyscale Digitisations

## ☛ **This is a work in progress** ☚

### Contributions are very welcome!

Please:

- keep the [issue tracker](http://github.com/amiaopensource/lenticular/issues) limited to bug reports and feature requests;
- use the [pull requests](http://github.com/amiaopensource/lenticular/pulls) to submit patches to both the code and its documentation.

Thank you!

### Short-term goals of the project

- Any idea for a better name than `lenticular`?
- I was told that a logo would be important.
- Understand and document the parameters.
- Upload test files on GitHub. Who can provide some for public usage?
- Fix error handling. In particular avoid the `Segmentation fault: 11` message, most probably due to wrongly set arrays’ indices.
- Allow full path for both the input and the output folder. In particular, the output folder has not to be nested inside the input folder, but can be chosen freely, e.g. on another hard-disk drive.
- Allow to choose the name of the output files.
- Add flags for mandatory parameters and allow free input order.
- Allow the file extensions `.tiff`, `.TIF` and `.TIFF` as well.
- Allow more TIFF flavours to be processed, _in primis_ `rgb48le`, because currently only `gray16le` is supported in `lenticular`. However, the additional interpolation-study tool does work with `rgb48le`. Therefore this should not be too hard to implement.
- Determine automatically the regex for parsing the greyscale frames of the input folder.
- The number of the first and of the last frame to process is needed only for testing a subset of the frames in the folder. If these parameters are not provided, then parse in ascending order all the files in the folder.
- Change the behaviour of the `-highRes` flag: replace with a resolution parameter and set the default to high resolution.

### Mid-term goals

- Could this be a project for the next AMIA Hackday?
- Verify that TIFF is really the best choice as processing format.
- Process any image format supported by FFmpeg.

### Long-term goals

- **CLI:** A command-line program that can be used by other software as well. That way, for example, FFmpeg (`ffmpeg` and `ffplay`) or `mpv` could be compiled with, or the restoration software `Diamant` could use it as a plug-in. The capability to use a library as a filter on the media player allows to show on-the-fly the lenticular colours while reading a greyscale file. This may be useull for quickly check if a film has been shot with the RGB filter on the lens or not.
- **GUI:** A stand-alone program – directly inspired by [AEO-Light](https://usc-imi.github.io/aeo-light/) – that allows to generate the colours of a digitised lenticular movie films, like _Kodacolor_ and _Agfacolor_. This allows to tune the parameters for a better result.

---

## An alpha release is available for testing purpose

The program depends on `libtiff`.

### How to compile the source

1. download the `lenticular` folder
1. open the terminal
1. `cd` into the folder
1. run `make && make clean`

### How to run the main tool

1. open the terminal
1. `cd` into the folder
1. run `./lenticular`

### Homebrew and Linuxbrew

Alternatively, you can install the modified `lenticular` tool by using Homebrew or Linuxbrew. Just run the command:

```
brew install amiaopensource/amiaos/lenticular
```

Once installed, run the main tool by the command:

```
lenticular
```

Brew installation and `lenticular` have been successfully tested on the following operating systems:

- **Linux:** Ubuntu 16.04.3 LTS and 14.04.5 LTS; Slackware 14.2; Debian 7.11
- **Mac:** macOS 10.13.3 and 10.12.6, OS X 10.11.6 and 10.10.5; 
- **Windows** Subsystem for Linux (aka Ubuntu 14.04.4 LTS)

### Parameters

```
lenticular 2018-03-24 alpha

lenticular [-help] [-highRes] [-profileRelThickness (float)]
  [-profileRelPosY (float)] [-relaxRaster]
  [-rasterSpacing (float)] [-troubleshoot]
  'inputDir' 'inputBaseName' 'startNo' 'endNo' 'outputDir'
```

#### Mandatory parameters

- **inputDir:** path to the input directory, e.g. `~/TEST/SOURCE_FILES/`
- **inputBaseName:** input base filename without the numbering, e.g. `greyscale_`
- **startNo:** number of the first file, e.g. `0001`
- **endNo:** number of the last file, e.g. `0057`
- **outputDir:** output directory, e.g. `NEW_FILES`

Please note that, like in the original code, as well as in the current version:

- the parameters must be passed in this order
- the file extension must be `.tif` and the extension `.tiff` is not allowed
- currently only `gray16le` is supported and not `rgb48le`
- in the example above the first input file is `~/TEST/SOURCE_FILES/greyscale_0001.tif`
- the output folder must be nested inside the input folder
- only the name of the output folder must be entered (with the path from the input folder on, if any, but not the full path)
- in the example above the last output file is `~/TEST/SOURCE_FILES/NEW_FILES/rgb_greyscale_0057.tif`
- inconsistent input may provoke a `Segmentation fault: 11` error

#### Optional parameters

- **-help**
- **-highRes:** set this flag for a high-resolution output
- **-profileRelThickness (float)**
- **-profileRelPosY (float)**
- **-relaxRaster**
- **-rasterSpacing (float)**
- **-troubleshoot**

#### FFmpeg commands to transform input to gray16le

As currently `lenticuar` does only process `gray16le` content, the FFmpeg commands to transcode in this format are provided.

##### Single-image based content

```
ffmpeg -f image2 -i INPUT_FOLDER/input_file_%08d.ext -pix_fmt gray16le OUTPUT_FOLDER/greyscale_%08d.tif
```

The following file formats of good digitisations in a 4K resolution give very nice results:
- TIFF `rgb48le`
- DPX 16-bit, 12-bit or 10-bit
- OpenEXR

##### Stream-image based content

```
ffmpeg -i input_file.ext -pix_fmt gray16le OUTPUT_FOLDER/greyscale_%08d.tif
```

We have tested so far the following stream-based file formats in 2K resolution:
- FFV1 `gbrp16le` and `yuv444p`
- uncompressed 10-bit
- ProRes 4444 and ProRes 422 HQ

### Additional tools

The original source code comes with the following two additional tools:

- `interpolationstudy`: pixel column interpolation study on `rgb48le`
- `framecropper`: crop center part of RGB frames

Note that we just have started to explore the additional tools.

We have added to the Homebrew/Linuxbrew formula options to install one or both of them. And, most importantly, please [report](../CONTRIBUTING.md) any findings.

#### Interpolation study

To install the interpolation-study tool via Homebrew or Linuxbrew, run the command:

```
brew install amiaopensource/amiaos/lenticular --with-interpolationstudy
```

Once installed, run the main tool by the command:

```
interpolationstudy
```

which should give:

```
interpolationstudy 2018-03-24 alpha
  RGB image pixel column interpolation study

interpolationstudy [--help] 'inputFileName'
```

Note that:

- the «option '--help' [is] ignored at the moment», as is any passed flag
- **inputFileName:** currently the input file must be at the same level than `interpolationstudy`. Hmm…
- when only a flag is passed, a `Segmentation fault: 11` occurs

#### Frame cropper

To install the frame-cropper tool via Homebrew or Linuxbrew, run the command:

```
brew install amiaopensource/amiaos/lenticular --with-framecropper
```

Once installed, run the main tool by the command:

```
framecropper
```

which should give:


```
framecropper 2018-03-24 alpha
  crop center part of RGB frames

framecropper 'width' 'height' 'inputBaseName' 'startNo' 'endNo' 'outputDir'
```

---

## Copyright, Licence and Disclaimer

Copyright (c) 2012 Joakim Reuteler  
Copyright (c) 2018 AMIA Open Source

The program is released under a [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.en.html) Licence and the documentation is released under a [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) Licence.

This work is provided «as is» without warranty or support of any kind.
