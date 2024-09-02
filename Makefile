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
# Location of the project directory and Makefiles
#
P := .
M := $(P)/.makefile

###################################################
# Project definition
#
include $(P)/Makefile.common
TARGET_NAME := $(PROJECT_NAME)
TARGET_DESCRIPTION := Open-source simulator for autonomous driving research
include $(M)/project/Makefile

###################################################
# Standalone application
#
SRC_FILES := src/main.cpp
INCLUDES += $(P) $(P)/src $(P)/include
INCLUDES += $(P)/external/MyLogger/include
VPATH := $(P)/src/Common $(P)/src/Scenario $(P)/src/Simulator

###################################################
# Set Libraries.
#
PKG_LIBS += sfml-graphics

###################################################
# Internal libs to compile
#
LIB_HIGHWAY_SCENARIO := $(call internal-lib,HighwayScenario)
LIB_HIGHWAY_CORE := $(call internal-lib,HighwayCore)
LIB_HIGHWAY_APPLICATION := $(call internal-lib,HighwayApplication)
INTERNAL_LIBS := $(LIB_HIGHWAY_APPLICATION) $(LIB_HIGHWAY_CORE) $(LIB_HIGHWAY_SCENARIO)
DIRS_WITH_MAKEFILE := $(P)/src/Scenario $(P)/src/Core $(P)/src/Application
#THIRDPART_LIBS := $(P)/external/MyLogger/build/libmylogger.a

###################################################
# Generic Makefile rules
#
include $(M)/rules/Makefile

###################################################
# Compile internal librairies in the correct order
#

$(LIB_HIGHWAY_CORE): $(P)/src/Core

$(LIB_HIGHWAY_SCENARIO): $(P)/src/Scenario

$(LIB_HIGHWAY_APPLICATION): $(P)/src/Application

$(P)/src/Application: $(P)/src/Core

$(P)/src/Core: $(P)/src/Scenario

###################################################
# Extra rules
#
all:: scenarios

###################################################
# Compile scenarios
#
SCENARIOS = $(sort $(dir $(wildcard ./Scenarios/*/.)))
.PHONY: scenarios
scenarios: | $(INTERNAL_LIBS)
	@$(call print-from,"Compiling scenarios",$(PROJECT_NAME),$(SCENARIOS))
	@for i in $(SCENARIOS);        \
	do                             \
		$(MAKE) -C $$i all;        \
	done;