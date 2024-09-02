#!/bin/bash -e
###############################################################################
### This script is called by (cd .. && make download-external-libs). It will
### git clone thirdparts needed for this project but does not compile them.
### It replaces git submodules that I dislike.
###############################################################################

### Basic logger for my GitHub C++ projects
### License: GPL-3.0
cloning Lecrapouille/MyLogger

### Lightweight C++ Signals and Slots implementation
### License: MIT
cloning adrg/sling

#### Library for unit conversion library
#### License: MIT
cloning nholthaus/units
cp units/include/units.h units/units.hpp

### JSON for Modern C++
### License: MIT
cloning nlohmann/json