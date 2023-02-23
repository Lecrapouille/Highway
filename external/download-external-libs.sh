#!/bin/bash -e
###############################################################################
### This script is called by (cd .. && make download-external-libs). It will
### git clone thirdparts needed for this project but does not compile them.
### It replaces git submodules that I dislike.
###############################################################################

source ../.makefile/download-external-libs.sh

### Basic logger for my GitHub C++ projects
### License: GPL-3.0
cloning Lecrapouille/MyLogger

#### Library for unit conversion library
#### License: MIT
cloning nholthaus/units
(cd units/include && mkdir units && cp units.h units/units.h)

#### Quick and simple implementation of the Perlin Noise gradient.
#### License: MIT
cloning daniilsjb/perlin-noise

#### Random for modern C++ with convenient API
#### License: MIT
cloning effolkronium/random
