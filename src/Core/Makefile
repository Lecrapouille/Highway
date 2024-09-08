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
P := ../..
M := $(P)/.makefile

###################################################
# Project definition
#
include $(P)/Makefile.common
TARGET_NAME := HighwayCore
TARGET_DESCRIPTION := Library Highway simulation
include $(M)/project/Makefile

###################################################
# Inform Makefile where to find *.cpp files
#
VPATH += $(P)/src

###################################################
# Inform Makefile where to find header files
#
INCLUDES += $(P) $(P)/src $(P)/include
INCLUDES += $(P)/external/MyLogger/include
INCLUDES += $(P)/external/json/include
INCLUDES += $(P)/external

###################################################
# Make the list of compiled files for the library
#
LIB_FILES += $(call rwildcard,$(P)/src/Core,*.cpp)
THIRDPART_LIBS := $(abspath $(P)/external/MyLogger/build/libmylogger.a)
# FIXME: SFML does not give static lib ;( ideally link against it
# to remove all sfml symbols (not because we need the graphical part
# but we use SFML for the maths).
# THIRDPART_LIBS := libsfml-graphics.a

###################################################
# Generic Makefile rules
#
include $(M)/rules/Makefile