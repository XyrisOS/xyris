#  ____             _        _____         _
# |  _ \ __ _ _ __ (_)_  __ |_   _|__  ___| |_ ___
# | |_) / _` | '_ \| \ \/ /   | |/ _ \/ __| __/ __|
# |  __/ (_| | | | | |>  <    | |  __/\__ \ |_\__ \.
# |_|   \__,_|_| |_|_/_/\_\   |_|\___||___/\__|___/
#
# Compiles the kernel source code located in the kernel folder.

# Designed by Keeton Feavel & Micah Switzer
# Copyright the Panix Contributors (c) 2019

# *****************************
# * Source Code & Directories *
# *****************************

# Directories & files
KERNEL  = kernel
SYSROOT	= ../sysroot
INCLUDE = $(SYSROOT)/usr/include

# Define the compilation flags for the tests.
CXXFLAGS +=            \
	-Wno-write-strings

CPPFLAGS +=                 \
	-I$(INCLUDE)/$(KERNEL)/ \
	-DTESTING
# Use the host's copy of GCC instead of the i686 cross compiler
# since it does not have a standard library (if compiled properly)
CXX 	:= $(shell command -v gcc)
# macOS does not support compiling for i386
# so we only add the -m32 flag for Linux hosts.
UNAME_S := $(shell uname -s)

run_tests: *.cpp
ifeq ($(UNAME_S), Linux)
	CXXFLAGS += -m32
endif
ifeq ($(UNAME_S), Darwin)
	@printf "\n$(COLOR_INFO)WARNING$(COLOR_NONE): Compiling for Darwin (amd64)\n\n"
endif
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $< ../$(KERNEL)/lib/bitmap.cpp

.PHONY: test
test: run_tests
	@./run_tests

.PHONY: clean
clean:
	@$(RM) run_tests
