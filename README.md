# Highway

A light open-source simulator for autonomous driving research. This project is developed in C++14. The GUI is made with the [SFML](https://www.sfml-dev.org/index-fr.php).

## Why another car simulator ?

This simulator has not the pretention to revolutionize the world of the simulation but just a personal challenge for offering an intuitive API for autonomous driving research because when I tried [Carla simulator](https://github.com/carla-simulator/carla), I did not like the following points:
- Hard to install (Unreal Engine, NVidia drivers, patches, need Docker ...) and takes giga bytes of binaries on the hard disk. The application is low to run.
- Very instable: lot of crashes, lot of issues on GitHub that are never fixed and auto-closed after X days without answers.
- The physical models of cars are not very realist: looks like playing the GTA III game.
- Poor API. Just read the first lines of the API: `carla.Actor: defines actors as anything that plays a role in the simulation or can be moved around. That includes: pedestrians, vehicles, sensors and traffic signs ...` but when reading their Actor methods: `close_door` ... We can interrogate if this API has been really tough.
- The documentation is sometimes hard to understand. I was personnaly stuck searching on the API what methods giving me dimensions of my car.

In my opinion, a simulator has to help the developper by offering the possibility to interact with all objects in the word. For example the simulator has to reply to question such as "give me the list of pedestrians around the car seen by the front camera" ... list that can be used to check at run time, for example, whith the list of pedestrians detected by your IA/sensors/ECU of your vehicle and compare it. This idea can be extended to any mobile/static object in the city: pedestrian, cars, roads, traffic signs, zebras, parking slot, spawning points ... all of them shall interact with the ego vehicle and share their states (ie traffic light: the color of the light, the content of any road signs ie "D20" ...).

Here a list of things a simulator can help:
- Collisions shall trigger events;
- a simulator should offer to the developper different physical models (kinematics, dynamics ...) that can be changed at run time or offering to the player the way to develop it's own model;
- the API shall offers blueprint/dimension of the car (wheelbase ...);
- the simulator can help the developper cheating: for example, apply a constant longitudinal velocity to the car if we do not want to simulate the brake and the accelerator pedals (ideally the API shall distinguish helper methods to twick the simulation from real physical behavior by explicit methods for example apply velocity on the right side of the car);
- the simulator should monitor vehicle states and save logs that be replayed or plot graphs (for example in Matlab or Julia);
- the simulator should display shapes to inform the developper: for example the arc for turning circle, show spawning points identifier, show arrows for the vehicle speed, acceleration, the car path ...

## Compilation / Installation

### Compilation

This code depends on:
- C++-17 compiler (g++, clang++)
- lib SFML for 2D graphism: https://www.sfml-dev.org/index-fr.php
- SWI Prolog: `sudo apt-get install swi-prolog`
- Optionally for stack tracing on segfaults: https://github.com/bombela/backward-cpp (can be disabled if undesired by
commenting some lines inside the Makefile).
- Google tests for unit tests.

To compile and install the standalone application:

```sh
git clone git@github.com:Lecrapouille/Highway.git --depth=1
make clean
make -j8
```

To compile a simulation file:

```sh
cd Simulations/SimpleParking/
make clean
make -j8
```

The compiled simulation file is in `Simulations/SimpleParking/build/simpleparking.so`.
This a pure shared library that can be opened by the standalone application with (`dlopen`).
See next section for more information.

To run the standalone application. A default scenario will be run:

```sh
./build/Highway
```

To run the standalone application and running a scenarion (shared library):

```sh
Highway simulation.so
```

### Installation

To install the application and scenarios on your operating system:

```sh
sudo make install
```

To run the standalone application installed on your system:

```sh
Highway
# or Highway simulation.so
```

### Non regression tests

To make unit tests and generate code coverage.

```sh
make check -j8
```

Or

```sh
cd tests
make coverage -j8
```

If you do not want generating reports.

```sh
cd tests
make -j8
./build/Highway-Tests
```

### Generate the documentation

To generate Doxygen documentation.

```sh
make doc
```

The generated documentation can be find in `doc/html`.

### Compress your work

To create a tar.gz of the project with management of name conflict concerning the tarball name. Compiled files, generated doc, git files and backup files are not stored in the tarball.

```sh
make tarball
```

## Create simulations

You can create more simulation files. The code is C++. You have to compiled the simulation file into
a shared library, the standalone application will extract symbols. For the moment you have to implement
the following C fonctions:

```C++
//! \brief C function returning your simulation name.
const char* simulation_name();

//! \brief C function taking the simulator as input (given by the application) and returning true when
//! the simulation shall halt (ie when the ego collides, timeout, outside the world ...).
bool halt_simulation_when(Simulator const& simulator);

//! \brief C fonction taking a city as input-output (given by the application) to allow your to creating
//! your desired city (ie roads, parkings, cars, pedestrians ...). You have to create your ego vehicle
//! with city.addEgo(...) and ie attach to it some sensors. This function shall return the ego car.
Car& create_city(City& city);
```

You have to compile it, for example:

```sh
cd Simulations/SimpleParking/
make clean
make -j8
```

Once compiled, you can pass it to the standalone application.

## Developper

For emacs users, you can type `M-x turn-on-iimage-mode` to show pictures linked in C++ comments. This will help you to understand the code.
The code source refers to mathematic formulas from the following documents cited in the bibliography section.

## Bibliography

A personal implementation of ego car doing self parking maneuvers (ie parallel) based on algorithms described in the following documents:
- (fr) `Estimation et contrôle pour le pilotage automatique de véhicule` by Sungwoo Choi
- (en) `Easy Path Planning and Robust Control for Automatic Parallel Parking` by Sungwoo CHOI, Clément Boussard, Brigitte d'Andréa-Novel.
Which is a summary of the previous document.
- (en) `Automatic Parking and Path Following Control for a Heavy-Duty Vehicle` by Joakim Mörhed and Filip Östman
- (en) `Automatic parking of self-driving car based on LIDAR` by Bijun Lee, Yang Wei, I. Yuan Guo.
- (en) `Flatness and motion Planning the Car with n-trailer` by Pierre Rouchon.

