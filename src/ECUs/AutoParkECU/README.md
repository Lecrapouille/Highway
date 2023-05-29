# Automatic Parallel Parking ECU

Parallel parking is the only implemented currently. Others (perpendicular, diagonal)
are present in the code but mathematics are not implemented.

Pics are refered in comments in the C++ code source. For emacs users, you can type
`M-x turn-on-iimage-mode` to show pictures linked in C++ comments. This will help you
to understand the code.

## Detail Designs

The Ego vehicle is driving along the road searching the first parking spot. It uses
a single "antenna" sensor.

![alt parking](doc/StateMachines/ParkingStrategyFig.png)

The state machine for the parking is:

![alt fsm_parking](doc/StateMachines/ParkingStateMachine.jpg)

It calls the state machine for scanning empty parking spots:

![alt fsm_scanner](doc/StateMachines/ScanStateMachine.jpg)

The parallel trajectory is computed with documents refered in this bibliography.

![alt paral_trajec](doc/Parallel/ParallelFinalStep.png)

Once the ego has found a trajectory to the empty spot:

![alt fsm_trajec](doc/StateMachines/ParkingStrategyFSM.png)

## Bibliography

The code source refers to mathematic formulas from the following documents.
Note PDF have been compressed to reduce this repo size. You can google them
to get their original version without loose of quality.

- ![fr](https://lecrapouille.github.io/icons/fr.png) `Estimation et contrôle pour le pilotage automatique de véhicule` by Sungwoo Choi
- ![en](https://lecrapouille.github.io/icons/en.png) `Easy Path Planning and Robust Control for Automatic Parallel Parking` by Sungwoo CHOI, Clément Boussard, Brigitte d'Andréa-Novel.
Which is a summary of the previous document.
- ![en](https://lecrapouille.github.io/icons/en.png) `Automatic Parking and Path Following Control for a Heavy-Duty Vehicle` by Joakim Mörhed and Filip Östman
- ![en](https://lecrapouille.github.io/icons/en.png) `Automatic parking of self-driving car based on LIDAR` by Bijun Lee, Yang Wei, I. Yuan Guo.
- (![en](https://lecrapouille.github.io/icons/en.png) `Flatness and motion Planning the Car with n-trailer` by Pierre Rouchon.
