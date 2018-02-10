# Digital Reconstruction of Lenticular Colours

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

- **Linux** (Ubuntu 16.04.3 LTS and 14.04.5 LTS; Slackware 14.2)
- **Mac** (OS X 10.10.5 and 10.11.6; macOS 10.12.6 and 10.13.3)
- **Windows** Subsystem for Linux (Ubuntu 14.04.4 LTS)

---

## Licence and Disclaimer

This work is released under a [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) Licence.

This work is provided «as is» without warranty or support of any kind.
