## 2021 Quentin Quadrat quentin.quadrat@gmail.com
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

TARGET_BIN = Drive

# make install
DESTDIR ?=
PREFIX ?= /usr
BINDIR := $(DESTDIR)$(PREFIX)/bin
LIBDIR := $(DESTDIR)$(PREFIX)/lib
DATADIR := $(DESTDIR)$(PREFIX)/share/$(TARGET_BIN)/data

# Search files
BUILD = build
VPATH = $(BUILD) src src/World src/Vehicle src/Utils src/Sensors src/SelfParking src/SelfParking/Trajectory src/Renderer
INCLUDES = -Isrc -Iinclude

# C++14 only because of std::make_unique not present in C++11)
STANDARD=--std=c++14 -g -O0

# Warnings
COMPIL_FLAGS = -Wall -Wextra -Wuninitialized -Wundef -Wunused       \
  -Wunused-result -Wunused-parameter -Wtype-limits                  \
  -Wcast-align -Wcast-qual -Wconversion -Wfloat-equal               \
  -Wpointer-arith -Wswitch-enum -pedantic -Wpacked -Wold-style-cast \
  -Wdeprecated -Wvariadic-macros -Wvla -Wsign-conversion

COMPIL_FLAGS += -Wno-switch-enum -Wno-undef -Wno-unused-parameter -Wno-old-style-cast -Wno-sign-conversion

# Compilation
CXXFLAGS = $(STANDARD) $(COMPIL_FLAGS) `pkg-config --cflags sfml-graphics`
LDFLAGS = `pkg-config --libs sfml-graphics` -lpthread -ldw
DEFINES = -DDATADIR=\"$(DATADIR)\" -DBACKWARD_HAS_DW=1

# File dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD)/$*.Td
POSTCOMPILE = mv -f $(BUILD)/$*.Td $(BUILD)/$*.d

# Object files
OBJS_VEHICLE = VehicleControl.o VehiclePhysics.o VehicleShape.o Vehicle.o
OBJS_UTILS = backward.o Collide.o 
OBJS_SIMULATION = Renderer.o Parking.o Simulation.o
OBJS_SENSORS = Radar.o
OBJS_TRAJECTORY = CarTrajectory.o CarPerpendicularTrajectory.o CarParallelTrajectory.o CarDiagonalTrajectory.o
OBJS_SELFPARKING = SelfParkingStateMachine.o SelfParkingScanParking.o SelfParkingVehicle.o

OBJS = $(OBJS_UTILS) $(OBJS_VEHICLE) $(OBJS_SENSORS) $(OBJS_TRAJECTORY) $(OBJS_SELFPARKING) $(OBJS_SIMULATION) main.o

ifeq ($(VERBOSE),1)
Q :=
else
Q := @
endif

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

# Install
install: $(TARGET_BIN) $(TARGET_LIB)
	@echo "Installing $(TARGET_BIN)"
	$(Q)mkdir -p $(BINDIR)
	$(Q)mkdir -p $(LIBDIR)
	$(Q)mkdir -p $(DATADIR)
	cp $(BUILD)/$(TARGET_BIN) $(BINDIR)
	cp $(BUILD)/$(TARGET_LIB) $(LIBDIR)
	cp -r data $(DATADIR)/..

# Delete compiled files
.PHONY: clean
clean:
	-rm -fr $(BUILD)

# Create the directory before compiling sources
$(OBJS): | $(BUILD)
$(BUILD):
	@mkdir -p $(BUILD)

# Create the dependency files
$(BUILD)/%.d: ;
.PRECIOUS: $(BUILD)/%.d

-include $(patsubst %,$(BUILD)/%.d,$(basename $(OBJS)))
