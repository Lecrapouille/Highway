# Auto-Parking

A personal implementation of ego car doing self parking maneuvers (ie parallel) based on algorithms described in the following documents:
- (fr) `Estimation et contrôle pour le pilotage automatique de véhicule` by Sungwoo Choi
- (en) `Easy Path Planning and Robust Control for Automatic Parallel Parking` by Sungwoo CHOI, Clément Boussard, Brigitte d'Andréa-Novel.
Which is a summary of the previous document.
- (en) `Automatic Parking and Path Following Control for a Heavy-Duty Vehicle` by Joakim Mörhed and Filip Östman
- (en) `Automatic parking of self-driving car based on LIDAR` by Bijun Lee, Yang Wei, I. Yuan Guo.
- (en) `Flatness and motion Planning the Car with n-trailer` by Pierre Rouchon.

License of the code source: Domain Public

The code source refers to mathematic formulas from these documents.

## Why another car simulator ?

When I tried Carla simulator, I did not like the following points:
- Hard to install (need docker) and takes Giga bytes of binaries on the hard disk.
- Very instable (lot of crashes, lot of issues never fixed by the main developpers with auto-closing bot when X days without answers have spent).
- The physical models of cars are not very realist: looks like playing the GTA III game.
- Poor API. Just get the 1st lines of the API: carla.Actor defines actors as anything that plays a role in the simulation or can be moved around. That includes: pedestrians, vehicles, sensors and traffic signs ... but when reading their methods: close_door. We can interrogate if this API has been really tough.

In my opinion, a simulator has to help the developper by offering the possibility to interact with all objects in the word (for example "give me the list of pedestrians around the car (or seen by the front camera)" ... list that can be used to check at run time if, for example, the IA detects correctly pedestrians). The idea can be extended to ant object in the city: pedestrian, cars, roads, traffic signs, zebras, parking slot, spawning points ... all shall interact with the ego vehicle. Collisions shall trigger events. A simulator should offer to the developper different physical models (kinematics, dynamics ...) that can be changed at run time or offer to the player to develop it's own model. The API shall offers blueprint/dimension of the car (wheelbase ...). The simulator can help the developper cheating: for example, apply a constant longitudinal velocity to the car if we do not want to simulate the brake and the accelerator pedals (ideally the API shall distinguish helper methods to twick the simulation from real physical behavior by explicit methods for example apply velocity on the right side of the car). Finally the simulator should monitor vehicle states and save logs that be replayed or plot graphs (for example in Matlab or Julia). The simulator should display shapes to inform the developper: for example the arc for turning circle, show spawning points identifier, show arrows for the vehicle speed, acceleration, the car path ...

This simulator has not the pretention to revolution the world of simulation but just wish that mainstream simulator offermore intuitive API.

## Installation

This code depends on:
- C++-14 compiler (g++, clang++)
- lib SFML for 2D graphism: https://www.sfml-dev.org/index-fr.php
- Optionally for stack tracing on segfaults: https://github.com/bombela/backward-cpp (can be disabled if undesired by
commenting some lines inside the Makefile).
- Google tests for unit tests.

Standalone application:
```sh
git clone git@github.com:Lecrapouille/Drive.git --depth=1
make clean
make -j8
sudo make install
```

Run the standalone application:
```sh
CarSimulator
```

Compile simulation file:
```sh
cd Simulations/SimpleParking/
make clean
make -j8
```

The simulation file is in `Simulations/SimpleParking/build/simpleparking.so`.
You can create more simulation files. For the moment implement these fonctions:

```C++
//! \brief Function returning the simulation name.
const char* simulation_name();

//! \brief Function taking the Simulator as input and returns true when
//! the simulation shall halt.
bool halt_simulation_when(Simulator const& simulator);

//! \brief C fonction taking a City as input and allows to create the
//! desired city. This function returns the ego car.
Car& create_city(City& city);
```

## Standalone application command line

Run the standalone application:
```sh
CarSimulator simpleparking.so
```

## Other commands:
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

## Notes

For emacs users, you can type `M-x turn-on-iimage-mode` to show pictures linked in C++ comments. This will help you to understand the code.
