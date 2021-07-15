#  ____             _        _____         _
# |  _ \ __ _ _ __ (_)_  __ |_   _|__  ___| |_ ___
# | |_) / _` | '_ \| \ \/ /   | |/ _ \/ __| __/ __|
# |  __/ (_| | | | | |>  <    | |  __/\__ \ |_\__ \.
# |_|   \__,_|_| |_|_/_/\_\   |_|\___||___/\__|___/
#
# Compiles the unit test source code located in the tests folder.
# TODO: This makefile does not properly account for main.cpp not
#       being located in a subdirectory. Same thing happens with
#       the kernel makefile, but it doesn't seem as affected for
#       some reason. We should figure out how to fix that.

# Designed by Keeton Feavel & Micah Switzer
# Copyright the Panix Contributors (c) 2019

# *****************************
# * Source Code & Directories *
# *****************************

# Directories & files
OUTPUT      := unit-test
BUILD_DIR   := $(BUILD_DIR)/$(OUTPUT)
CATCH2_DIR  := $(THIRDPARTY_DIR)/catch2
CATCH2_HDR  := $(CATCH2_DIR)/catch.hpp
CATCH2_URL  := "https://api.github.com/repos/catchorg/Catch2/releases/latest"

# Use the host's copy of GCC instead of the i686 cross compiler
# since it does not have a standard library (if compiled properly)
CXX := $(shell which gcc)

# *******************
# * Toolchain Flags *
# *******************

CXXFLAGS =                  \
	-fprofile-arcs          \
	-ftest-coverage

# For some reason we have to specifically link
# against libstdc++ and libm. Not sure why, but
# it works so I won't ask questions.
LDFLAGS =                   \
	--coverage              \
	-lstdc++                \
	-lgcov                  \
	-lgcc                   \
	-lm

CPPFLAGS +=                 \
	-I$(ROOT)/tests/        \
	-I$(THIRDPARTY_DIR)     \
	-I$(KERNEL_DIR)         \
	-DTESTING

# *******************
# * Catch2 Download *
# *******************

.PHONY: $(CATCH2_HDR)
$(CATCH2_HDR):
# Check if header has been downloaded
ifeq (,$(wildcard $(CATCH2_HDR)))
# Make directory, cd into it, and download
	@printf "$(COLOR_COM)(GET)$(COLOR_NONE)\t$(CATCH2_HDR)\n"
	@mkdir -p $(CATCH2_DIR)
	@curl -s $(CATCH2_URL) \
	| grep "catch.hpp" \
	| cut -d : -f 2,3 \
	| tr -d \" \
	| wget -P $(CATCH2_DIR) -qi - \
	|| true
endif

# *************************
# * Kernel Source Objects *
# *************************

# Source
CPP_SRC  = $(shell find . -type f -name "*.cpp" | sed "s|^\./||")
# Headers
CPP_HDR  = $(shell find $(KERNEL_DIR) -type f -name "*.hpp" | sed "s|^\./||")
HEADERS  = $(CPP_HDR) $(CATCH2_HDR)

# All objects
OBJ_CPP      := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC))
OBJ          := $(OBJ_CPP)
# Object directories, mirroring source
OBJ_DIRS = $(addprefix $(BUILD_DIR), $(shell find . -type d | sed "s|^\.||"))
# Create object file directories
OBJ_DIRS_MAKE := mkdir -p $(OBJ_DIRS)
# Dependency files
DEP = $(OBJ_CPP:%.o=%.d)
# All files (source, header, etc.)
ALLFILES = $(CPP_SRC) $(HEADERS)
# Include all .d files
-include $(DEP)

# C++ source -> object
$(BUILD_DIR)/%.o: %.cpp $(HEADERS)
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(CXX)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(WARNINGS) -std=c++17 -MMD -c -o $@ $<
# Compile tests and run
$(OUTPUT): $(PRODUCTS_DIR)/$(OUTPUT)
$(PRODUCTS_DIR)/$(OUTPUT): $(OBJ)
	@mkdir -p $(PRODUCTS_DIR)
	@printf "$(COLOR_COM)(LD)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(CXX) -o $@ $(OBJ) $(LDFLAGS)
	@printf "$(COLOR_COM)(OBJCP)$(COLOR_NONE)\t$(shell basename $@)\n"
