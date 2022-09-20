##=====================================================================
## https://github.com/Lecrapouille/Highway
## Highway: Open-source simulator for autonomous driving research.
## Copyright 2021 -- 2022 Quentin Quadrat <lecrapouille@gmail.com>
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
## along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.
##=====================================================================

TARGET_BIN = Highway-Tests

# Search files
BUILD = build
VPATH = $(BUILD) . ../src ../src/World ../src/Vehicle ../src/Utils ../src/Sensors ../src/SelfParking ../src/SelfParking/Trajectories ../src/Renderer
INCLUDES = -I../src -I.

# C++14 (only because of std::make_unique not present in C++11)
STANDARD=--std=c++14

# Compilation flags
COMPIL_FLAGS = -Wall -Wextra -Wuninitialized -Wundef -Wunused       \
  -Wunused-result -Wunused-parameter -Wtype-limits                  \
  -Wcast-align -Wcast-qual -Wconversion -Wfloat-equal               \
  -Wpointer-arith -Wswitch-enum -pedantic -Wpacked -Wold-style-cast \
  -Wdeprecated -Wvariadic-macros -Wvla -Wsign-conversion
COMPIL_FLAGS += -Wno-switch-enum -Wno-undef -Wno-unused-parameter \
  -Wno-old-style-cast -Wno-sign-conversion

# Project flags
CXXFLAGS = $(STANDARD) $(COMPIL_FLAGS)
LDFLAGS = -lpthread
DEFINES = -DDATADIR=\"$(DATADIR)\"

# Lib SFML https://www.sfml-dev.org/index-fr.php
CXXFLAGS += `pkg-config --cflags sfml-graphics`
LDFLAGS += `pkg-config --libs sfml-graphics`

# Google tests
CXXFLAGS += `pkg-config --cflags gtest gmock`
LDFLAGS += `pkg-config --libs gtest gmock`

# Pretty print the stack trace https://github.com/bombela/backward-cpp
# You can comment these lines if backward-cpp is not desired
CXXFLAGS += -g -O0
LDFLAGS += -ldw
DEFINES += -DBACKWARD_HAS_DW=1
OBJS_DEBUG += backward.o

# Code coverage
CXXFLAGS += --coverage
LDFLAGS += --coverage
COVERAGE_RAPPORT=rapport.info
COVERAGE_DIR=doc
COVERAGE_TARGET=logexporter

# File dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD)/$*.Td
POSTCOMPILE = mv -f $(BUILD)/$*.Td $(BUILD)/$*.d

# Desired compiled files
OBJS_VEHICLE = VehicleControl.o VehiclePhysics.o VehicleShape.o Vehicle.o
OBJS_UTILS = $(OBJS_DEBUG) Collide.o
OBJS_SIMULATION = Renderer.o Parking.o Simulation.o
OBJS_SENSORS = Radar.o
OBJS_TRAJECTORY = Trajectory.o PerpendicularTrajectory.o ParallelTrajectory.o DiagonalTrajectory.o
OBJS_SELFPARKING = SelfParkingStateMachine.o SelfParkingScanParkedCars.o SelfParkingVehicle.o
OBJS_TU = TestVehicle.o Tests.o

OBJS = $(OBJS_UTILS) $(OBJS_VEHICLE) $(OBJS_SENSORS) $(OBJS_TRAJECTORY) $(OBJS_SELFPARKING) $(OBJS_SIMULATION) $(OBJS_TU)

# Verbosity control
ifeq ($(VERBOSE),1)
Q :=
else
Q := @
endif

# Compile the unit tests
all: $(TARGET_BIN)

# Run the tests and generate the code coverage
check: all
	@echo "Runing tests"
	./$(BUILD)/$(TARGET_BIN) || echo ""
	lcov --quiet --directory .. -c -o $(COVERAGE_RAPPORT)
	lcov --quiet --remove $(COVERAGE_RAPPORT) '/usr*' 'external/*' 'tests/*' -o $(COVERAGE_RAPPORT)
	genhtml --quiet -o $(COVERAGE_DIR) -t "$(COVERAGE_TARGET)" $(COVERAGE_RAPPORT)
	xdg-open $(COVERAGE_DIR)/index.html >/dev/null

# Link the target
$(TARGET_BIN): $(OBJS)
	@echo "Linking $@"
	$(Q)cd $(BUILD) && $(CXX) $(INCLUDES) -o $(TARGET_BIN) $(OBJS) $(LDFLAGS)

# Compile C++ source files
%.o : %.cpp $(BUILD)/%.d Makefile
	@echo "Compiling $<"
	$(Q)$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $(abspath $<) -o $(abspath $(BUILD)/$@)
	@$(POSTCOMPILE)

# Compile C source files
%.o : %.c $(BUILD)/%.d Makefile
	@echo "Compiling $<"
	$(Q)$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $(abspath $<) -o $(abspath $(BUILD)/$@)
	@$(POSTCOMPILE)

# Delete compiled files
.PHONY: clean
clean:
	$(Q)-rm -fr $(BUILD)

# Delete compiled files and backup files
.PHONY: veryclean
veryclean: clean
	$(Q)find -name "*~" -print -delete
	$(Q)-rm -fr doc
	$(Q)-$(MAKE) -C tests veryclean

# Create the directory before compiling sources
$(OBJS): | $(BUILD)
$(BUILD):
	@mkdir -p $(BUILD)

# Create the dependency files
$(BUILD)/%.d: ;
.PRECIOUS: $(BUILD)/%.d

-include $(patsubst %,$(BUILD)/%.d,$(basename $(OBJS)))
