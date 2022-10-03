# Creat scenarios for Highway

Create here your C++ scenario for the standalone application Highway. The code of your scenario shall be compiled as a shared library that can be loaded either through the command line or through the GUI menu.

Shared library is currently the most easy way to create shareable simulation files without needed the help of any scripting langage. Scenario could be hot reloaded (currently in gestation because not yet working).

## Steps to create a new scenario

- Create a folder holding at least one C++ file and a Makefile.
- Implement function depicted in the [API.hpp](API.hpp) in the C++ file.
- Create a Makefile calling the [Makefile.scenario](Makefile.scenario) for compiling the scenario. The Makefile is only 4 lines: file name of the scenario, its description (optional), `LIB_OBJS` holding the list of your c++ compiled files (ending with `.o`) and include the Makefile.scenario.
- Call `make` the shared library has been copied into [data/Scenarios](../data/Scenarios).
- Note that the Makefile for compiling Highway (on the root of the project) also compiles scenarios.
