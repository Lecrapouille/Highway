## 2021 -- 2022 Quentin Quadrat quentin.quadrat@gmail.com
##
## This is free and unencumbered software released into the public domain.
##
## Anyone is free to copy, modify, publish, use, compile, sell, or
## distribute this software, either in source code form or as a compiled
## binary, for any purpose, commercial or non-commercial, and by any
## means.
##
## In jurisdictions that recognize copyright laws, the author or authors
## of this software dedicate any and all copyright interest in the
## software to the public domain. We make this dedication for the benefit
## of the public at large and to the detriment of our heirs and
## successors. We intend this dedication to be an overt act of
## relinquishment in perpetuity of all present and future rights to this
## software under copyright law.
##
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
## EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
## MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
## IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
## OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
## ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
## OTHER DEALINGS IN THE SOFTWARE.
##
## For more information, please refer to <https://unlicense.org>

TARGET_BIN = AutoPark

# Needed for the command: make install
DESTDIR ?=
PREFIX ?= /usr
BINDIR := $(DESTDIR)$(PREFIX)/bin
LIBDIR := $(DESTDIR)$(PREFIX)/lib
DATADIR := $(DESTDIR)$(PREFIX)/share/$(TARGET_BIN)/data

# Compilation searching files
BUILD = build
VPATH = $(BUILD) src src/Application src/Math src/Simulator/Sensors \
  src/Simulator src/Simulator/City src/Simulator/Vehicle \
  src/Renderer src/Simulator/Vehicle/VehiclePhysicalModels src/Common \
  src/Simulation src/Simulation/SelfParking
INCLUDES = -Isrc -Isrc/Simulator

# C++14 (only because of std::make_unique not present in C++11)
STANDARD=--std=c++14

# Compilation flags
COMPIL_FLAGS = -Wall -Wextra -Wuninitialized -Wundef -Wunused       \
  -Wunused-result -Wunused-parameter -Wtype-limits -Wshadow                 \
  -Wcast-align -Wcast-qual -Wconversion -Wfloat-equal               \
  -Wpointer-arith -Wswitch-enum -Wpacked -Wold-style-cast \
  -Wdeprecated -Wvariadic-macros -Wvla -Wsign-conversion
COMPIL_FLAGS += -Wno-switch-enum -Wno-undef -Wno-unused-parameter \
  -Wno-old-style-cast -Wno-sign-conversion -Wcast-function-type

# Project flags
CXXFLAGS = $(STANDARD) $(COMPIL_FLAGS)
LDFLAGS = -lpthread
DEFINES = -DDATADIR=\"$(DATADIR)\" -DZOOM=0.01f

# Lib SFML https://www.sfml-dev.org/index-fr.php
CXXFLAGS += `pkg-config --cflags sfml-graphics`
LDFLAGS += `pkg-config --libs sfml-graphics`

# Pretty print the stack trace https://github.com/bombela/backward-cpp
# You can comment these lines if backward-cpp is not desired
CXXFLAGS += -g -O0
LDFLAGS += -ldw
DEFINES += -DBACKWARD_HAS_DW=1
OBJS_DEBUG += backward.o

# Header file dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD)/$*.Td
POSTCOMPILE = mv -f $(BUILD)/$*.Td $(BUILD)/$*.d

# Desired compiled files
OBJS += Monitoring.o Collide.o Components.o Drawable.o Renderer.o
OBJS += VehicleBluePrint.o VehicleShape.o TricycleKinematic.o
OBJS += Radar.o
OBJS += Car.o Trailer.o SelfParkingCar.o
OBJS += Pedestrian.o Parking.o BluePrints.o City.o
OBJS += GUIMainMenu.o GUISimulation.o

OBJS += SelfParkingComponent.o CarParkedScanner.o
#OBJS += Trajectory.o PerpendicularTrajectory.o ParallelTrajectory.o DiagonalTrajectory.o
#OBJS += SelfParkingStateMachine.o SelfParkingScanParkedCars.o SelfParkingVehicle.o
OBJS += Simulator.o Simulation.o main.o

# Verbosity control
ifeq ($(VERBOSE),1)
Q :=
else
Q := @
endif

# Compile the target
all: $(TARGET_BIN) $(TARGET_LIB)

# Link the target
$(TARGET_BIN): $(OBJS)
	@echo "Linking $@"
	$(Q)cd $(BUILD) && $(CXX) $(INCLUDES) -o $(TARGET_BIN) $(OBJS) $(LDFLAGS)

# Create the shared library
$(TARGET_LIB): $(LIB_OBJS)
	@echo "Library $@"
	$(Q)cd $(BUILD) && $(CXX) -shared -o $(TARGET_LIB) $(LIB_OBJS) $(LDFLAGS)

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

# Install the project
.PHONY: install
install: $(TARGET_BIN) $(TARGET_LIB)
	@echo "Installing $(TARGET_BIN)"
	$(Q)mkdir -p $(BINDIR)
	$(Q)mkdir -p $(LIBDIR)
	$(Q)mkdir -p $(DATADIR)
	cp $(BUILD)/$(TARGET_BIN) $(BINDIR)
	cp $(BUILD)/$(TARGET_LIB) $(LIBDIR)
	cp -r data $(DATADIR)/..

# Do unit tests and code coverage
.PHONY: check
check:
	@echo "Compiling unit tests"
	$(Q)$(MAKE) -C tests check

# Create the documentation
.PHONY: doc
doc:
	$(Q)doxygen Doxyfile

# Create the tarball
.PHONY: tarball
tarball:
	$(Q)./.targz.sh $(PWD) $(TARGET_BIN)

# Compile LaTeX documentation
.PHONY: latex
latex:
	$(Q)cd doc/IA/ && latex IA.tex && latex IA.tex && dvipdf IA.dvi

# Delete compiled files
.PHONY: clean
clean:
	$(Q)-rm -fr $(BUILD)

# Delete compiled files and backup files
.PHONY: veryclean
veryclean: clean
	$(Q)-rm -fr *~ .*~
	$(Q)find src -name "*~" -print -delete
	$(Q)-rm -fr doc/html

# Create the directory before compiling sources
$(OBJS): | $(BUILD)
$(BUILD):
	@mkdir -p $(BUILD)

# Create the dependency files
$(BUILD)/%.d: ;
.PRECIOUS: $(BUILD)/%.d

# Header file dependencies
-include $(patsubst %,$(BUILD)/%.d,$(basename $(OBJS)))
