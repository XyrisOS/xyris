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
OUTPUT      := unit-test
INCLUDE     := $(SYSROOT_DIR)/usr/include
KERNEL_SRC  := $(ROOT)/$(KERNEL)

# Use the host's copy of GCC instead of the i686 cross compiler
# since it does not have a standard library (if compiled properly)
CXX := $(shell command -v gcc)

# C / C++
C_SRC    = $(shell find . -type f -name "*.c" | sed "s|^\./||")
CPP_SRC  = $(shell find . -type f -name "*.cpp" | sed "s|^\./||")
# Headers
C_HDR    = $(shell find $(INCLUDE) -type f -name "*.h" | sed "s|^\./||")
CPP_HDR  = $(shell find $(INCLUDE) -type f -name "*.hpp" | sed "s|^\./||")
HEADERS  = $(CPP_HDR) $(C_HDR)

# *******************
# * Toolchain Flags *
# *******************

CXXFLAGS =                   \
	-Wno-write-strings

LDFLAGS =                    \
	-lgcc

CPPFLAGS +=                  \
	-I$(KERNEL_SRC)          \
	-I$(INCLUDE)/kernel/     \
	-DTESTING

# *************************
# * Kernel Source Objects *
# *************************

# All objects
OBJ_C        := $(patsubst %.c,   $(BUILD_DIR)/%.o, $(C_SRC))
OBJ_CPP      := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC))
OBJ          := $(OBJ_CPP) $(OBJ_C)
# Object directories, mirroring source
OBJ_DIRS = $(addprefix $(BUILD_DIR), $(shell find . -type d | sed "s|^\.||"))
# Create object file directories
OBJ_DIRS_MAKE := mkdir -p $(OBJ_DIRS)
# Dependency files
DEP = $(OBJ_CPP:%.o=%.d) $(OBJ_C:%.o=%.d)
# All files (source, header, etc.)
ALLFILES = $(C_SRC) $(CPP_SRC) $(HEADERS)
# Include all .d files
-include $(DEP)

# C source -> object
$(BUILD_DIR)/%.o: %.c $(HEADERS)
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(CC)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(CC) $(CPPFLAGS) $(WARNINGS) $(CWARNINGS) -std=c17 -MMD -c -o $@ $<
# C++ source -> object
$(BUILD_DIR)/%.o: %.cpp $(HEADERS)
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(CXX)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(CXX) $(CPPFLAGS) $(WARNINGS) $(CXXFLAGS) -std=c++17 -MMD -c -o $@ $<
# Compile tests and run
$(OUTPUT): $(PRODUCTS_DIR)/$(OUTPUT)
$(PRODUCTS_DIR)/$(OUTPUT): $(OBJ)
	@mkdir -p $(PRODUCTS_DIR)
	@printf "$(COLOR_COM)(LD)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(CXX) -o $@ $(OBJ) $(LDFLAGS)
	@printf "$(COLOR_COM)(OBJCP)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(PRODUCTS_DIR)/$(OUTPUT)
