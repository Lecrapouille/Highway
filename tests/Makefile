##=====================================================================
## Drive: A basic car simulation.
## Copyright 2021 Quentin Quadrat <quentin.quadrat@gmail.com>
##
## This file is part of Drive.
##
## Drive is free software: you can redistribute it and/or modify it
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

TARGET_BIN = Drive-TU

# make install
DESTDIR ?=
PREFIX ?= /usr
BINDIR := $(DESTDIR)$(PREFIX)/bin
LIBDIR := $(DESTDIR)$(PREFIX)/lib
DATADIR := $(DESTDIR)$(PREFIX)/share/$(TARGET_BIN)/data

# Search files
BUILD = build
VPATH = $(BUILD) ../src ../src/Car ../src/World ../src/SFML ../src/Utils
INCLUDES = -I../src -I../src/Car -I../src/World -I../src/SFML -I../src/Utils

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
CXXFLAGS = $(STANDARD) $(COMPIL_FLAGS) `pkg-config --cflags sfml-graphics gtest gmock` --coverage
LDFLAGS = `pkg-config --libs sfml-graphics gtest gmock` -lpthread -ldw  --coverage
DEFINES = -DDATADIR=\"$(DATADIR)\" -DBACKWARD_HAS_DW=1

# File dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD)/$*.Td
POSTCOMPILE = mv -f $(BUILD)/$*.Td $(BUILD)/$*.d

# Object files
OBJS = backward.o Collide.o CarControl.o CarPhysics.o CarShape.o CarTrajectory.o CarPerpendicularTrajectory.o CarParallelTrajectory.o CarDiagonalTrajectory.o Parking.o Renderer.o
OBJS += TestCar.o Tests.o

ifeq ($(VERBOSE),1)
Q :=
else
Q := @
endif

COVERAGE_RAPPORT=rapport.info
COVERAGE_DIR=doc
COVERAGE_TARGET=logexporter

all: $(TARGET_BIN)

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
	-rm -fr $(BUILD)

# Create the directory before compiling sources
$(OBJS): | $(BUILD)
$(BUILD):
	@mkdir -p $(BUILD)

# Create the dependency files
$(BUILD)/%.d: ;
.PRECIOUS: $(BUILD)/%.d

-include $(patsubst %,$(BUILD)/%.d,$(basename $(OBJS)))
