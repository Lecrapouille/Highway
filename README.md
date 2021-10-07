# Drive

A personal implementation of car doing an auto parking (ie parallel) based on algorithms described in the following documents:
- (fr) 'Estimation et contrôle pour le pilotage automatique de véhicule" by Sungwoo Choi
- (en) "Easy Path Planning and Robust Control for Automatic Parallel Parking" by Sungwoo CHOI, Clément Boussard, Brigitte d’Andréa-Novel.
Which is a summary of the previous document.
- (en) "Automatic Parking and Path Following Control for a Heavy-Duty Vehicle" by Joakim Mörhed and Filip Östman
- (en) "Automatic parking of self-driving car based on lidar" by Bijun Lee, Yang Wei, I. Yuan Guo.

License: Domain Public

# Installation

Depends on:
- C++-14 compiler (g++, clang++)
- lib SFML for 2D graphism: https://www.sfml-dev.org/index-fr.php
- Optionally for stack tracing on segfaults: https://github.com/bombela/backward-cpp (can be disabled if undesired by
commenting some lines inside the Makefile).
- Google tests for unit tests.

```sh
cd Drive
make clean
make -j8
./build/Drive
```

Other commands:
```sh
make tarball
```

To create a tar.gz of the project with management of name conflict concerning the tarball name. Compiled files, generated doc, git files and backup files are not stored in the tarball.

```sh
make doc
```

To generate Doxygen documentation.

```sh
make check
```

To make unit tests and generate code coverage.


# Notes

For emacs users, you can type `M-x turn-on-iimage-mode` to show pictures linked in C++ comments. This will help you to understand the code.
