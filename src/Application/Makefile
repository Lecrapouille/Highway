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
TARGET_NAME := HighwayApplication
TARGET_DESCRIPTION := Library Highway Application
include $(M)/project/Makefile

###################################################
# Pass the flag -export-dynamic to the ELF linker,
# on targets that support it. This instructs the
# linker to add all symbols, not only used ones,
# to the dynamic symbol table. This option is needed
# for some uses of dlopen or to allow obtaining
#backtraces from within a program.
LINKER_FLAGS += -rdynamic

###################################################
# Inform Makefile where to find *.cpp files
#
VPATH += $(P)/src

###################################################
# Inform Makefile where to find header files
#
INCLUDES += $(P) $(P)/src $(P)/include
INCLUDES += $(P)/external/MyLogger/include

###################################################
# Make the list of compiled files for the library
#
LIB_FILES += $(call rwildcard,$(P)/src/Application,*.cpp)
# Optional but this force to add symbols inside the library
LIB_FILES += $(call rwildcard,$(P)/src/Core/Common,*.cpp)

###################################################
# Set Libraries.
#
THIRDPART_LIBS := $(abspath $(P)/external/MyLogger/build/libmylogger.a)

###################################################
# Generic Makefile rules
#
include $(M)/rules/Makefile