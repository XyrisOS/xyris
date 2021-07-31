#  ____             _        _  __                    _
# |  _ \ __ _ _ __ (_)_  __ | |/ /___ _ __ _ __   ___| |
# | |_) / _` | '_ \| \ \/ / | ' // _ \ '__| '_ \ / _ \ |
# |  __/ (_| | | | | |>  <  | . \  __/ |  | | | |  __/ |
# |_|   \__,_|_| |_|_/_/\_\ |_|\_\___|_|  |_| |_|\___|_|
#
# Compiles the kernel source code.

# Designed by Keeton Feavel & Micah Switzer
# Copyright the Panix Contributors (c) 2019

BUILD_DIR := $(BUILD_DIR)/kernel

# *****************************
# * Source Code & Directories *
# *****************************

# The `| sed "s|^\./||"` command is included to remove the leading
# ./ from all of the paths so that the objects are generated
# properly. Otherwise the object directory is "obj." and not "obj"

# Assembly
ATT_SRC  = $(shell find . -type f -name "*.s" | sed "s|^\./||")
NASM_SRC = $(shell find . -type f -name "*.S" | sed "s|^\./||")
# C / C++
C_SRC    = $(shell find . -type f -name "*.c" | sed "s|^\./||")
CPP_SRC  = $(shell find . -type f -name "*.cpp" | sed "s|^\./||")
# Headers
C_HDR    = $(shell find $(KERNEL_DIR) -type f -name "*.h" | sed "s|^\./||")
CPP_HDR  = $(shell find $(KERNEL_DIR) -type f -name "*.hpp" | sed "s|^\./||")
HEADERS  = $(CPP_HDR) $(C_HDR)
# Libraries
LIBS_A   = $(shell find $(LIBRARY_DIR) -type f -name "*.a" -exec basename {} \;)
LIBS     = $(addprefix -l:, $(LIBS_A))

# *******************
# * Toolchain Flags *
# *******************

CPPFLAGS +=                \
	-I ${KERNEL_DIR}       \
	-I $(THIRDPARTY_DIR)

# Add compiler analysis to the kernel compilation
# Added here instead of in the root-level makefile
# since this option isn't recognized by Clang, which
# may be the host's compiler, thus making unit tests
# not compiler properly.
CXXFLAGS +=                \
	-fanalyzer             \

LDFLAGS +=                 \
	-T arch/i386/linker.ld \
	-L $(LIBRARY_DIR)/**   \
	$(LIBS)                \

# *************************
# * Kernel Source Objects *
# *************************

# All objects
OBJ_CRTBEGIN := $(shell $(CC) -print-file-name=crtbegin.o)
OBJ_CRTEND   := $(shell $(CC) -print-file-name=crtend.o)
OBJ_CRTI     := $(BUILD_DIR)/arch/i386/crti.o # hard-coded for now
OBJ_CRTN     := $(BUILD_DIR)/arch/i386/crtn.o # hard-coded for now
OBJ_C        := $(patsubst %.c,   $(BUILD_DIR)/%.o, $(C_SRC))
OBJ_CPP      := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(CPP_SRC))
OBJ_ASM_ALL  := $(patsubst %.s,   $(BUILD_DIR)/%.o, $(ATT_SRC)) \
                $(patsubst %.S,   $(BUILD_DIR)/%.o, $(NASM_SRC))
OBJ_ASM      := $(filter-out $(OBJ_CRTI) $(OBJ_CRTN),$(OBJ_ASM_ALL))
OBJ          := $(OBJ_CRTI) $(OBJ_CRTBEGIN) $(OBJ_CPP) $(OBJ_C) $(OBJ_ASM) $(OBJ_CRTEND) $(OBJ_CRTN)
# Object directories, mirroring source
OBJ_DIRS = $(addprefix $(BUILD_DIR), $(shell find . -type d | sed "s|^\.||"))
# Create object file directories
OBJ_DIRS_MAKE := mkdir -p $(OBJ_DIRS)
# Dependency files
DEP = $(OBJ_CPP:%.o=%.d) $(OBJ_C:%.o=%.d)
# Include all .d files
-include $(DEP)

# C source -> object
$(BUILD_DIR)/%.o: %.c $(HEADERS)
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(CC)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(CWARNINGS) -std=c17 -MMD -c -o $@ $<
# C++ source -> object
$(BUILD_DIR)/%.o: %.cpp $(HEADERS)
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(CXX)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(CXX) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) -std=c++17 -MMD -c -o $@ $<
# NASM assembly -> object
$(BUILD_DIR)/%.o: %.S
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(NASM)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(NASM) -f elf32 -o $@ $<
# Kernel object
$(KERNEL): $(PRODUCTS_DIR)/$(KERNEL)
$(PRODUCTS_DIR)/$(KERNEL): $(OBJ)
	@mkdir -p $(PRODUCTS_DIR)
	@printf "$(COLOR_COM)(LD)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(CXX) -o $@ $(OBJ) $(LDFLAGS)
	@printf "$(COLOR_COM)(OBJCP)$(COLOR_NONE)\t$(shell basename $@)\n"
	@$(OBJCP) --only-keep-debug $(PRODUCTS_DIR)/$(KERNEL) $(PRODUCTS_DIR)/$(SYMBOLS)
