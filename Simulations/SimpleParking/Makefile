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

TARGET_LIB = simpleparking.so

# Needed for the command: make install
DESTDIR ?=
PREFIX ?= /usr
BINDIR := $(DESTDIR)$(PREFIX)/bin
LIBDIR := $(DESTDIR)$(PREFIX)/lib
DATADIR := $(DESTDIR)$(PREFIX)/share/$(TARGET_BIN)/data

# Compilation searching files
BUILD = build
VPATH = $(BUILD)
INCLUDES = -I.. -I../../src -I../../src/Simulator

# C++14 (only because of std::make_unique not present in C++11)
STANDARD=--std=c++14

# Compilation flags
COMPIL_FLAGS = -Wall -Wextra -Wuninitialized -Wundef -Wunused       \
  -Wunused-result -Wunused-parameter -Wtype-limits -Wshadow                 \
  -Wcast-align -Wcast-qual -Wconversion -Wfloat-equal               \
  -Wpointer-arith -Wswitch-enum -Wpacked -Wold-style-cast \
  -Wdeprecated -Wvariadic-macros -Wvla -Wsign-conversion -Wno-unknown-warning-option
COMPIL_FLAGS += -Wno-switch-enum -Wno-undef -Wno-unused-parameter \
  -Wno-old-style-cast -Wno-sign-conversion -Wcast-function-type

# Project flags
CXXFLAGS += $(STANDARD) $(COMPIL_FLAGS)
LDFLAGS += -lpthread
DEFINES += -DZOOM=0.01f
CXXFLAGS +=
LDFLAGS += -L$(abspath ../../build) -lcarsimulator

# Header file dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD)/$*.Td
POSTCOMPILE = mv -f $(BUILD)/$*.Td $(BUILD)/$*.d

# Desired compiled files
OBJS += SimpleParking.o

# Verbosity control
ifeq ($(VERBOSE),1)
Q :=
else
Q := @
endif

# Compile the target
all: $(TARGET_LIB)

# Create the shared library
$(TARGET_LIB): $(OBJS)
	@echo "Library $@"
	$(Q)cd $(BUILD) && $(CXX) -shared -o $(TARGET_LIB) $(OBJS) $(LDFLAGS)

# Compile C++ source files
%.o : %.cpp $(BUILD)/%.d Makefile
	@echo "Compiling $<"
	$(Q)$(CXX) $(DEPFLAGS) -fPIC $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $(abspath $<) -o $(abspath $(BUILD)/$@)
	@$(POSTCOMPILE)

# Compile C source files
%.o : %.c $(BUILD)/%.d Makefile
	@echo "Compiling $<"
	$(Q)$(CXX) $(DEPFLAGS) -fPIC $(CXXFLAGS) $(INCLUDES) $(DEFINES) -c $(abspath $<) -o $(abspath $(BUILD)/$@)
	@$(POSTCOMPILE)

# Delete compiled files
.PHONY: clean
clean:
	$(Q)-rm -fr $(BUILD)

# Create the directory before compiling sources
$(OBJS): | $(BUILD)
$(BUILD):
	@mkdir -p $(BUILD)

# Create the dependency files
$(BUILD)/%.d: ;
.PRECIOUS: $(BUILD)/%.d

# Header file dependencies
-include $(patsubst %,$(BUILD)/%.d,$(basename $(OBJS)))
