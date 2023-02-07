##=====================================================================
## https://github.com/Lecrapouille/Highway
## Highway: Open-source simulator for autonomous driving research.
## Copyright 2021 -- 2023 Quentin Quadrat <lecrapouille@gmail.com>
##
## This file is part of Highway.
##
## Highway is free software: you can redistribute it and/or modify it
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
## along with Highway.  If not, see <http://www.gnu.org/licenses/>.
##=====================================================================

###################################################
# Project definition
#
PROJECT = Highway
TARGET = $(PROJECT)
DESCRIPTION = Open-source simulator for autonomous driving research
STANDARD = --std=c++17
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
INCLUDES += -I$(P)/src

###################################################
# Inform Makefile where to find *.cpp and *.o files
#
VPATH += $(P)/src $(P)/src/Common $(P)/src/Application $(P)/src/Math	\
  $(P)/src/Sensors $(P)/src/Simulation $(P)/src/City $(P)/src/Vehicle	\
  $(P)/src/Renderer $(P)/src/Vehicle/VehiclePhysicalModels		\
  $(P)/src/ECUs/AutoParkECU $(P)/src/ECUs/AutoParkECU/Trajectories

###################################################
# Project defines
#
DEFINES = -DDATADIR=\"$(DATADIR)\"

###################################################
# Reduce warnings
#
DEFINES += -Wno-switch-enum -Wno-undef -Wno-unused-parameter -Wno-pedantic
DEFINES += -Wno-old-style-cast -Wno-sign-conversion -Wno-deprecated-copy-dtor

###################################################
# Thirdpart: MyLogger
#
THIRDPART_LIBS += $(abspath $(THIRDPART)/MyLogger/build/libmylogger.a)
INCLUDES += -I$(THIRDPART)/MyLogger/include

###################################################
# Thirdpart: SI units
#
INCLUDES += -I$(THIRDPART)/units/include

###################################################
# Make the list of compiled files for the library
#
LIB_OBJS += FilePath.o Collide.o SpatialHashGrid.o Prolog.o
LIB_OBJS += FontManager.o Drawable.o Renderer.o
LIB_OBJS += VehicleBluePrint.o VehicleShape.o TricycleKinematic.o
LIB_OBJS += Radar.o Antenna.o
LIB_OBJS += Car.o Trailer.o
LIB_OBJS += Pedestrian.o Parking.o Network.o Road.o BluePrints.o City.o
LIB_OBJS += Application.o GUIMainMenu.o GUISimulation.o GUILoadSimulMenu.o
LIB_OBJS += Trajectory.o ParallelTrajectory.o AutoParkECU.o
# PerpendicularTrajectory.o ParallelTrajectory.o DiagonalTrajectory.o
LIB_OBJS += Demo.o Scenario.o Simulator.o

###################################################
# Make the list of compiled files for the application
#
OBJS += $(LIB_OBJS) main.o

###################################################
# Set Libraries. For knowing which libraries
# is needed please read the external/README.md file.
#
PKG_LIBS += sfml-graphics swipl
LINKER_FLAGS += -lpthread -ldl

# C++17 filesystem: on older compiler you need
# the experimental lib
ifneq "$(GCC_GREATER_V9)" "1"
LINKER_FLAGS += -lstdc++fs
endif

###################################################
# MacOS X
#
ifeq ($(ARCHI),Darwin)
BUILD_MACOS_APP_BUNDLE = 1
APPLE_IDENTIFIER = lecrapouille
MACOS_BUNDLE_ICON = data/Highway.icns
LINKER_FLAGS += -framework CoreFoundation
endif

###################################################
# Entry point to call other makefiles compiling all
# scenarios.
#
SCENARIOS = $(sort $(dir $(wildcard ./Scenarios/*/.)))

###################################################
# Compile the project, the static and shared libraries
# and scenarios.
#
.PHONY: all
all: $(TARGET) $(STATIC_LIB_TARGET) $(SHARED_LIB_TARGET) $(PKG_FILE) scenarios

###################################################
# Compile scenarios
#
.PHONY: scenarios
scenarios: | $(STATIC_LIB_TARGET) $(SHARED_LIB_TARGET)
	@$(call print-from,"Compiling scenarios",$(PROJECT),$(SCENARIOS))
	@for i in $(SCENARIOS);        \
	do                             \
		$(MAKE) -C $$i all;        \
	done;

###################################################
# Compile and launch unit tests and generate the code coverage html report.
#
.PHONY: unit-tests
unit-tests:
	@$(call print-simple,"Compiling unit tests")
	@$(MAKE) -C tests coverage

###################################################
# Compile and launch unit tests and generate the code coverage html report.
#
.PHONY: check
check: unit-tests

ifeq ($(ARCHI),Linux)
###################################################
# Install project. You need to be root.
#
.PHONY: install
install: $(TARGET) $(STATIC_LIB_TARGET) $(SHARED_LIB_TARGET) $(PKG_FILE)
	@$(call INSTALL_BINARY)
	@$(call INSTALL_DOCUMENTATION)
	@$(call INSTALL_PROJECT_LIBRARIES)
	@$(call INSTALL_PROJECT_HEADERS)
endif

###################################################
# Clean the whole project.
#
.PHONY: veryclean
veryclean: clean
	@rm -fr cov-int $(PROJECT).tgz *.log foo 2> /dev/null
	@(cd tests && $(MAKE) -s clean)
	@$(call print-simple,"Cleaning","$(PWD)/doc/html")
	@rm -fr $(THIRDPART)/*/ doc/html 2> /dev/null
	@rm -f data/Scenarios/*.$(SO) 2> /dev/null
	@for i in $(SCENARIOS);        \
	do                             \
		$(MAKE) -C $$i clean;      \
	done;

###################################################
# Sharable informations between all Makefiles
include $(M)/Makefile.footer
