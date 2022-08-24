##=====================================================================
## TimedPetriNetEditor: A timed Petri net editor.
## Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
##
## This file is part of PetriEditor.
##
## PetriEditor is free software: you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
##=====================================================================

###################################################
# Project definition
#
PROJECT = Drive
TARGET = $(PROJECT)
DESCRIPTION = Timed Petri Net Editor
STANDARD = --std=c++14
BUILD_TYPE = debug

###################################################
# Location of the project directory and Makefiles
#
P := .
M := $(P)/.makefile
include $(M)/Makefile.header

###################################################
# Inform Makefile where to find header files
#
INCLUDES += -I$(P)/src -I$(P)/src/Simulator

###################################################
# Inform Makefile where to find *.cpp and *.o files
#
VPATH += $(P)/src $(P)/src/Application $(P)/src/Math $(P)/src/Simulator/Sensors	\
  $(P)/src/Simulator $(P)/src/Simulator/City $(P)/src/Simulator/Vehicle		\
  $(P)/src/Renderer $(P)/src/Simulator/Vehicle/VehiclePhysicalModels		\
  $(P)/src/Common $(P)/src/ECUs/AutoParkECU

###################################################
# Project defines
#
DEFINES = -DDATADIR=\"$(DATADIR)\" -DZOOM=0.01f

###################################################
# Reduce warnings
#
DEFINES += -Wno-switch-enum -Wno-undef -Wno-unused-parameter
DEFINES += -Wno-old-style-cast -Wno-sign-conversion -Wno-deprecated-copy-dtor

###################################################
# Make the list of compiled files used both by the
# library and application
#
COMMON_OBJS = Howard.o KeyBindings.o Application.o PetriNet.o PetriEditor.o

###################################################
# Make the list of compiled files for the library
#
LIB_OBJS += Path.o Collide.o SpatialHashGrid.o Monitoring.o Components.o
LIB_OBJS += FontManager.o Drawable.o Renderer.o
LIB_OBJS += VehicleBluePrint.o VehicleShape.o TricycleKinematic.o
LIB_OBJS += Radar.o
LIB_OBJS += Car.o Trailer.o
LIB_OBJS += Pedestrian.o Parking.o BluePrints.o City.o
LIB_OBJS += Application.o GUIMainMenu.o GUISimulation.o
LIB_OBJS += AutoParkECU.o CarParkedScanner.o
#LIB_OBJS += Trajectory.o PerpendicularTrajectory.o ParallelTrajectory.o DiagonalTrajectory.o
#LIB_OBJS += SelfParkingStateMachine.o SelfParkingScanParkedCars.o SelfParkingVehicle.o
LIB_OBJS += Simulator.o

###################################################
# Make the list of compiled files for the application
#
OBJS += $(LIB_OBJS) main.o

###################################################
# Set Libraries. For knowing which libraries
# is needed please read the external/README.md file.
#
PKG_LIBS = sfml-graphics

LINKER_FLAGS += -lpthread -ldl

###################################################
# MacOS X
#
ifeq ($(ARCHI),Darwin)
BUILD_MACOS_APP_BUNDLE = 1
APPLE_IDENTIFIER = lecrapouille
MACOS_BUNDLE_ICON = data/Drive.icns
LINKER_FLAGS += -framework CoreFoundation
endif

###################################################
# Compile the project, the static and shared libraries
.PHONY: all
all: $(TARGET) $(STATIC_LIB_TARGET) $(SHARED_LIB_TARGET) $(PKG_FILE)

###################################################
# Compile and launch unit tests and generate the code coverage html report.
.PHONY: unit-tests
unit-tests:
	@$(call print-simple,"Compiling unit tests")
	@$(MAKE) -C tests coverage

###################################################
# Compile and launch unit tests and generate the code coverage html report.
.PHONY: check
check: unit-tests

ifeq ($(ARCHI),Linux)
###################################################
# Install project. You need to be root.
.PHONY: install
install: $(TARGET) $(STATIC_LIB_TARGET) $(SHARED_LIB_TARGET) $(PKG_FILE)
	@$(call INSTALL_BINARY)
	@$(call INSTALL_DOCUMENTATION)
	@$(call INSTALL_PROJECT_LIBRARIES)
	@$(call INSTALL_PROJECT_HEADERS)
endif

###################################################
# Clean the whole project.
.PHONY: veryclean
veryclean: clean
	@rm -fr cov-int $(PROJECT).tgz *.log foo 2> /dev/null
	@(cd tests && $(MAKE) -s clean)
	@$(call print-simple,"Cleaning","$(PWD)/doc/html")
	@rm -fr $(THIRDPART)/*/ doc/html 2> /dev/null

###################################################
# Sharable informations between all Makefiles
include $(M)/Makefile.footer
