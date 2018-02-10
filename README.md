# Digital Restoration of Lenticular Colours from Greyscale Images

This piece of software restores the colours from digitisations of lenticular movie films (like Kodacolor or Agfacolor), made without applying the RGB filter on the lens, i.e. by using the greyscale images.

This repository contains:

- the original doLCE source code by Joakim Reuteler, [Digital Humanities Lab](http://dhlab.unibas.ch/dolce/), University of Basel, Switzerland 2012
- a work-in-progress code (2018)

---

## Original doLCE Code

[doLCE](doLCE/README) – do Lenticular film Color rEconstruction

Copyright (c) 2012 Joakim Reuteler

This program is free software: you can redistribute it and/or modify it under the terms of the [GNU General Public License](https://www.gnu.org/licenses/#GPL) version 3 as published by the Free Software Foundation.

### Bibliography

Joakim Reuteler and Rudolf Gschwind ([Digital Humanities Lab](http://dhlab.unibas.ch/dolce/), University of Basel, Switzerland): «Die Farben des Riffelfilms. Digitale Farbrekonstruktion von Linsenrasterfilm», in: _Rundbrief Fotografie_, May 2014

### Caveats

- The original code is buggy.
- The original code can process TIFF images only and the extension must be `.tif`, not `.tiff`.
- The original code can process Kodacolor, Kodak’s flavour of lenticular film, but not [Agfacolor](http://zauberklang.ch/filmcolors/timeline-entry/1262/), Agfa’s flavour.

---

## Work-in-Progress Code

An alpha of a [modified code](lenticular/README.md) is available for testing purpose.
