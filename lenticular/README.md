# Digital Restoration of Lenticular Colours from Greyscale Images

## ☛ **This is a work in progress!** ☚

### Long-term goals of the project

- **CLI:** A command-line programme with a library that can be used e.g. from FFmpeg (`ffmpeg` and `ffplay`) or `mpv` as well.
- **GUI:** A stand-alone programme, directly inspired by [AEO-Light](https://usc-imi.github.io/aeo-light/), that allows to generate the colours of digitised lenticular movie films, like _Kodacolor_ and _Agfacolor_.

### Mid-term goal

- Could this be a project for the next AMIA Hackday?
- Process any image format supported by FFmpeg.

### Short-term goal

- Any idea for a better name than `doLCE` for the modified version?
- Use the standard `(x, y)` order of coordinates, instead of Monolith 2’s inverted `(y, x)` one.

---

## A pre-alpha is available for testing purpose

### How to compile the source

1. download the `lenticular` folder
1. open the terminal
1. `cd` into the folder
1. run `make && make clean`

### How to run the tool

1. open the terminal
1. `cd` into the folder
1. run `./doLCE`

### Homebrew and Linuxbrew

Alternatively, you can install the modified `doLCE` tool by using Homebrew or Linuxbrew: run the command:

```
brew install amiaopensource/amiaos/lenticular
```

Once installed, run the tool by the command:

```
doLCE
```

Brew installation and `doLCE` have been successfully tested on the following operating systems:

- **Linux:** Ubuntu 16.04.3 LTS and 14.04.5 LTS; Slackware 14.2; Debian 7.11
- **Mac:** OS X 10.10.5 and 10.11.6; macOS 10.12.6 and 10.13.3
- **Windows:** Subsystem for Linux (aka Ubuntu 14.04.4 LTS)

### Parameters

```
modified doLCE (do Lenticular film Color rEconstruction)

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

Please note that, like in the original code, also in the current version:

- the file extension must be `.tif` and the extension `.tiff` is not allowed
- in the example above the first input file is `~/TEST/SOURCE_FILES/greyscale_0001.tif`
- the output directory must be nested inside the input directory
- only the name of the output folder must be given
- in the example above the first output file is `~/TEST/SOURCE_FILES/NEW_FILES/rgb_greyscale_0001.tif`

---

## Licence and Disclaimer

This work is released under a [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) Licence.

This work is provided «as is» without warranty or support of any kind.
