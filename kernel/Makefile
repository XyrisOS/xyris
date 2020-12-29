#  ____             _        _  __                    _
# |  _ \ __ _ _ __ (_)_  __ | |/ /___ _ __ _ __   ___| |
# | |_) / _` | '_ \| \ \/ / | ' // _ \ '__| '_ \ / _ \ |
# |  __/ (_| | | | | |>  <  | . \  __/ |  | | | |  __/ |
# |_|   \__,_|_| |_|_/_/\_\ |_|\_\___|_|  |_| |_|\___|_|
#
# Compiles the kernel source code.

# Designed by Keeton Feavel & Micah Switzer
# Copyright the Panix Contributors (c) 2019

# *****************************
# * Source Code & Directories *
# *****************************

# Directories & files
BUILD   = ../obj
KERNEL  = kernel
SYMBOLS = $(KERNEL).sym
PRODUCT = ../dist
LIBRARY = ../libs
SYSROOT	= ../sysroot
INCLUDE = $(SYSROOT)/usr/include

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
C_HDR    = $(shell find $(INCLUDE) -type f -name "*.h" | sed "s|^\./||")
CPP_HDR  = $(shell find $(INCLUDE) -type f -name "*.hpp" | sed "s|^\./||")
HEADERS  = $(CPP_HDR) $(C_HDR)
#Libraries
LIBS_A   = $(shell find $(LIBRARY) -type f -name "*.a")
LIBS     = $(addprefix -l:, $(LIBS_A))

# *******************
# * Toolchain Flags *
# *******************

CPPFLAGS +=               \
	-I ${INCLUDE}/kernel/ \
	-I ${INCLUDE}/boot/

LDFLAGS +=                 \
	-T arch/i386/linker.ld \
	$(LIBS)                \

# *************************
# * Kernel Source Objects *
# *************************

# All objects
OBJ_CRTBEGIN := $(shell $(CC) -print-file-name=crtbegin.o)
OBJ_CRTEND   := $(shell $(CC) -print-file-name=crtend.o)
OBJ_CRTI     := $(BUILD)/arch/i386/crti.o # hard-coded for now
OBJ_CRTN     := $(BUILD)/arch/i386/crtn.o # hard-coded for now
OBJ_C        := $(patsubst %.c,   $(BUILD)/%.o, $(C_SRC))
OBJ_CPP      := $(patsubst %.cpp, $(BUILD)/%.o, $(CPP_SRC))
OBJ_ASM_ALL  := $(patsubst %.s,   $(BUILD)/%.o, $(ATT_SRC)) \
                $(patsubst %.S,   $(BUILD)/%.o, $(NASM_SRC))
OBJ_ASM      := $(filter-out $(OBJ_CRTI) $(OBJ_CRTN),$(OBJ_ASM_ALL))
OBJ          := $(OBJ_CRTI) $(OBJ_CRTBEGIN) $(OBJ_CPP) $(OBJ_C) $(OBJ_ASM) $(OBJ_CRTEND) $(OBJ_CRTN)
# Object directories, mirroring source
OBJ_DIRS = $(addprefix $(BUILD), $(shell find . -type d | sed "s|^\.||"))
# Create object file directories
OBJ_DIRS_MAKE := mkdir -p $(OBJ_DIRS)
# Dependency files
DEP = $(OBJ_CPP:%.o=%.d) $(OBJ_C:%.o=%.d)
# All files (source, header, etc.)
ALLFILES = $(ATT_SRC) $(NASM_SRC) $(C_SRC) $(CPP_SRC) $(HEADERS)
# Include all .d files
-include $(DEP)

# C source -> object
$(BUILD)/%.o: %.c $(HEADERS)
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(CC)$(COLOR_NONE)\t$@\n"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(CWARNINGS) -std=c17 -MMD -c -o $@ $<
# C++ source -> object
$(BUILD)/%.o: %.cpp $(HEADERS)
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(CXX)$(COLOR_NONE)\t$@\n"
	@$(CXX) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) -MMD -c -o $@ $<
# GAS assembly -> object
$(BUILD)/%.o: %.s
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(AS)$(COLOR_NONE)\t$@\n"
	@$(AS) $(ASFLAGS) -o $@ $<
# NASM assembly -> object
$(BUILD)/%.o: %.S
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(NASM)$(COLOR_NONE)\t$@\n"
	@$(NASM) -f elf32 -o $@ $<
# Kernel object
$(KERNEL): $(PRODUCT)/$(KERNEL)
$(PRODUCT)/$(KERNEL): $(OBJ)
	@mkdir -p $(PRODUCT)
	@printf "$(COLOR_COM)(LD)$(COLOR_NONE)\t$@\n"
	@$(LD) -o $@ $(OBJ) $(LDFLAGS)
	@printf "$(COLOR_COM)(OBJCP)$(COLOR_NONE)\t$@\n"
	@$(OBJCP) --only-keep-debug $(PRODUCT)/$(KERNEL) $(PRODUCT)/$(SYMBOLS)
