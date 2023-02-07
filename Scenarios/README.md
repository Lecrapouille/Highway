# Create scenarios for Highway

You can create your own scenarios for [Highway](https://github.com/Lecrapouille/Highway) separatly from
the standalone application. This document will explain you how to do it.

## Prerequisites

The code source of your simulation file shall be C++ and include "the [API header](../include/Highway/API.hpp)
and link against the `libhighway.a`.

You have to compile your simulation file(s) into a shared library and place it inside [../data/Scenarios/](../data/Scenarios/).
The code of your scenario shall be compiled into a shared library. Scenarios can be loaded either through the
command line of the standalone application or through the GUI menu.

While not architecture dependent, shared libraries are currently the easier way to create shareable simulation files
without the help of any scripting langage.

Currently in gestation because not yet totally functional: scenarios could be hot reloaded (meaning: compiled and
Highway will restarts it without restarting the standalone application.

## API

The standalone application will extract symbols. For the moment you have to implement
the following C fonctions:

- `const char* simulation_name()` to return the scenario name.
- `Car& simulation_create_city(Simulator& simulator, City& city)` Create your ideal city (roads, traffic ...)
  and return your pimped ego vehicle (with sensors and ECUs).
- `void simulation_react_to(Simulator& simulator, size_t const key)` Make your simulation reacts to keyboard
  events (key pressed).
- `bool simulation_halt_when(Simulator const& simulator)` allows to stop the simulation under conditions.
The function returns:
   - `CONTINUE_SIMULATION` to not abort the simulation.
   - `ABORT_SIMULATION` when you have desired to halt the simulation.
   - You can use the macro `HALT_SIMULATION_WHEN(condition, message)` to halt the simulation: `condition`
   can be "your ego vehicle collides an object", or "simulation timeout" ... `message` is the text displayed
   on your GUI explaining the reason.

## Examples

You can read the code source of the [demo](src/Simulation/Demo.cpp) or the scenarios given in this folder.

## Steps to create a new scenario

- Create a folder holding at least one C++ file and a Makefile:
  - The C++ file shall implements function depicted in the API.hpp.
  - The Makefile shall calls the [Makefile.scenario](Makefile.scenario) for compiling the scenario. The Makefile is only 4 lines: file name of the scenario, its description (optional), `LIB_OBJS` holding the list of your c++ compiled files (ending with `.o`) and include "the Makefile.scenario.
- Call `make` and the shared library will be created and then copied into [../data/Scenarios](../data/Scenarios).
- Note that the Makefile for compiling Highway (on the root of the project) also compiles scenarios.
