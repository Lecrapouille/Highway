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

TARGET_BIN = Drive-TU

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
