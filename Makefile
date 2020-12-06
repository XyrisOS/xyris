#  ____             _        _  __                    _
# |  _ \ __ _ _ __ (_)_  __ | |/ /___ _ __ _ __   ___| |
# | |_) / _` | '_ \| \ \/ / | ' // _ \ '__| '_ \ / _ \ |
# |  __/ (_| | | | | |>  <  | . \  __/ |  | | | |  __/ |
# |_|   \__,_|_| |_|_/_/\_\ |_|\_\___|_|  |_| |_|\___|_|
#
# Compiles the kernel source code located in the kernel folder.

# Designed by Keeton Feavel & Micah Switzer
# Copyright the Panix Contributors (c) 2019

PROJ_NAME = panix

# Makefile flags
# prevent make from showing "entering/leaving directory" messages
MAKEFLAGS 	  += --no-print-directory
GIT_VERSION   := "$(shell git describe --abbrev=8 --dirty --always --tags)"

# ******************************
# * Compiler Output Formatting *
# ******************************

COLOR_COM  = \033[0;34m
COLOR_OK   = \033[0;32m
COLOR_NONE = \033[m

# *****************************
# * Source Code & Directories *
# *****************************

# Directories & files
BUILD   = obj
LIBRARY = libs
KERNEL  = kernel
ISOIMG  = $(PROJ_NAME).iso
SYMBOLS = $(KERNEL).sym
PRODUCT = dist
SYSROOT	= sysroot
INCLUDE = $(SYSROOT)/usr/include

# Assembly
ATT_SRC  = $(shell find $(KERNEL) -type f -name "*.s")
NASM_SRC = $(shell find $(KERNEL) -type f -name "*.S")
# C / C++
C_SRC    = $(shell find $(KERNEL) -type f -name "*.c")
CPP_SRC  = $(shell find $(KERNEL) -type f -name "*.cpp")
# Headers
C_HDR    = $(shell find $(INCLUDE) -type f -name "*.h")
CPP_HDR  = $(shell find $(INCLUDE) -type f -name "*.hpp")
HEADERS  = $(CPP_HDR) $(C_HDR)
# Libraries
LIB_DIRS := $(shell find $(LIBRARY) -mindepth 1 -maxdepth 1 -type d)
LIBS_A   = $(shell find $(LIBRARY) -type f -name "*.a")
LIBS     = $(addprefix -l:,$(LIBS_A))

# *******************
# * i686 Toolchains *
# *******************

# Compilers/Assemblers/Linkers
export NASM    := $(shell command -v nasm)
export AS      := $(shell command -v i686-elf-as)
export AR      := $(shell command -v i686-elf-ar)
export CC      := $(shell command -v i686-elf-gcc)
export CXX     := $(shell command -v i686-elf-g++)
export LD      := $(shell command -v i686-elf-ld)
export OBJCP   := $(shell command -v i686-elf-objcopy)
export MKGRUB  := $(shell command -v grub-mkrescue)

# *******************
# * Toolchain Flags *
# *******************

# Warning flags
# (Disable unused functions warning)
WARNINGS :=              \
	-Wall                \
	-Werror              \
	-Wextra              \
	-Winline             \
	-Wshadow             \
	-Wcast-align         \
	-Wno-long-long       \
	-Wpointer-arith      \
	-Wwrite-strings      \
	-Wredundant-decls    \
	-Wno-unused-function \
	-Wmissing-declarations
# Flags to be added later
#   -Wconversion
# C only warnings
CWARNINGS :=             \
	-Wnested-externs     \
	-Wstrict-prototypes  \
	-Wmissing-prototypes \
# C flags (include directory)
CFLAGS :=                   \
	-m32                    \
	-nostdlib               \
	-nodefaultlibs          \
	-ffreestanding          \
	-fstack-protector-all   \
	-fno-builtin            \
	-fno-omit-frame-pointer \
	${PANIX_CFLAGS}         \
	${WARNINGS}
# C++ flags
CXXFLAGS :=                 \
	${PANIX_CXXFLAGS}       \
	-fpermissive            \
	-fno-rtti               \
	-fno-exceptions         \
	-fno-use-cxa-atexit     \
	-std=c++17
# C / C++ pre-processor flags
CPPFLAGS :=                       \
	${PANIX_CPPFLAGS}             \
	-D VERSION=\"$(GIT_VERSION)\" \
	-I ${SYSROOT}/usr/include/kernel/
# Assembler flags
ASFLAGS :=           \
	${PANIX_ASFLAGS} \
	--32
# Linker flags
LDFLAGS =                         \
	${PANIX_LDFLAGS}              \
	-m elf_i386                   \
	-T kernel/arch/i386/linker.ld \
	-L.                           \
	$(LIBS)                       \
	-lgcc

# ************************
# * Kernel Build Targets *
# ************************

# Release build
# This will be build by default since it
# is the first target in the Makefile
release: CXXFLAGS += -O3 -mno-avx
release: CFLAGS += -O3 -mno-avx
release: $(KERNEL)

# Debug build
debug: CXXFLAGS += -DDEBUG -g
debug: CFLAGS += -DDEBUG -g
debug: $(KERNEL)

# *************************
# * Kernel Source Objects *
# *************************

# All objects
OBJ_C   = $(patsubst $(KERNEL)/%.c,   $(BUILD)/%.o, $(C_SRC))
OBJ_CPP = $(patsubst $(KERNEL)/%.cpp, $(BUILD)/%.o, $(CPP_SRC))
OBJ_ASM = $(patsubst $(KERNEL)/%.s,   $(BUILD)/%.o, $(ATT_SRC)) \
          $(patsubst $(KERNEL)/%.S,   $(BUILD)/%.o, $(NASM_SRC))
OBJ     = $(OBJ_CPP) $(OBJ_C) $(OBJ_ASM)
# Object directories, mirroring source
OBJ_DIRS = $(subst $(KERNEL), $(BUILD), $(shell find $(KERNEL) -type d))
# Create object file directories
OBJ_DIRS_MAKE := mkdir -p $(OBJ_DIRS)
# Dependency files
DEP = $(OBJ_CPP:%.o=%.d) $(OBJ_C:%.o=%.d)
# All files (source, header, etc.)
ALLFILES = $(ATT_SRC) $(NASM_SRC) $(C_SRC) $(CPP_SRC) $(HEADERS)
# Include all .d files
-include $(DEP)

# C source -> object
$(BUILD)/%.o: $(KERNEL)/%.c $(HEADERS)
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(CC)$(COLOR_NONE)\t$@\n"
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(CWARNINGS) -std=c17 -MMD -c -o $@ $<
# C++ source -> object
$(BUILD)/%.o: $(KERNEL)/%.cpp $(HEADERS)
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(CXX)$(COLOR_NONE)\t$@\n"
	@$(CXX) $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) -MMD -c -o $@ $<
# GAS assembly -> object
$(BUILD)/%.o: $(KERNEL)/%.s
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(AS)$(COLOR_NONE)\t$@\n"
	@$(AS) $(ASFLAGS) -o $@ $<
# NASM assembly -> object
$(BUILD)/%.o: $(KERNEL)/%.S
	@$(OBJ_DIRS_MAKE)
	@printf "$(COLOR_COM)(NASM)$(COLOR_NONE)\t$@\n"
	@$(NASM) -f elf32 -o $@ $<
# Kernel object
$(PRODUCT)/$(KERNEL): $(LIBS_A) $(OBJ)
	@mkdir -p $(PRODUCT)
	@printf "$(COLOR_COM)(LD)$(COLOR_NONE)\t$@\n"
	@$(LD) -o $@ $(OBJ) $(LDFLAGS)
	@printf "$(COLOR_COM)(OBJCP)$(COLOR_NONE)\t$@\n"
	@$(OBJCP) --only-keep-debug $(PRODUCT)/$(KERNEL) $(PRODUCT)/$(SYMBOLS)
# Kernel (Linked With Libraries)
.PHONY: $(KERNEL)
$(KERNEL): export CFLAGS += $(CWARNINGS)
$(KERNEL):
	@for dir in $(LIB_DIRS); do        \
        $(MAKE) -C $$dir $(PROJ_NAME); \
    done
	@$(MAKE) $(PRODUCT)/$(KERNEL)

# ********************************
# * Kernel Distribution Creation *
# ********************************

# Create bootable ISO
iso: $(PRODUCT)/$(KERNEL)
	@mkdir -p iso/boot/grub
	@cp $(PRODUCT)/$(KERNEL) iso/boot/
	@cp boot/grub.cfg iso/boot/grub/grub.cfg
	@$(MKGRUB) -o $(PRODUCT)/$(ISOIMG) iso
	@rm -rf iso

# *************************
# * Virtual Machine Flags *
# *************************

# QEMU flags
QEMU_FLAGS =        \
    -m 4G           \
    -rtc clock=host \
    -vga std        \
    -serial stdio
QEMU_ARCH = i386
# Virtualbox flags
VM_NAME	= $(PROJ_NAME)-box
VBOX_VM_FILE=$(PRODUCT)/$(VM_NAME)/$(VM_NAME).vbox
# VM executable locations
VBOX = $(shell command -v VBoxManage)
QEMU = $(shell command -v qemu-system-$(QEMU_ARCH))

# ***************************
# * Virtual Machine Testing *
# ***************************

# Run Panix in QEMU
.PHONY: run
run: $(PRODUCT)/$(KERNEL)
	$(QEMU)                      \
	-kernel $(PRODUCT)/$(KERNEL) \
	$(QEMU_FLAGS)

# Create Virtualbox VM
.PHONY: vbox-create
vbox-create: $(PRODUCT)/$(ISOIMG)
	$(VBOX) createvm --register --name $(VM_NAME) --basefolder $(shell pwd)/$(PRODUCT)
	$(VBOX) modifyvm $(VM_NAME)                 \
	--memory 256 --ioapic on --cpus 2 --vram 16 \
	--graphicscontroller vboxvga --boot1 disk   \
	--audiocontroller sb16 --uart1 0x3f8 4      \
	--uartmode1 file $(shell pwd)/com1.txt
	$(VBOX) storagectl $(VM_NAME) --name "DiskDrive" --add ide --bootable on
	$(VBOX) storageattach $(VM_NAME) --storagectl "DiskDrive" --port 1 --device 1 --type dvddrive --medium $(PRODUCT)/$(ISOIMG)

.PHONY: vbox-create
vbox: vbox-create
	$(VBOX) startvm --putenv --debug $(VM_NAME)

# Open the connection to qemu and load our kernel-object file with symbols
.PHONY: debugger
debugger: $(PRODUCT)/$(KERNEL)
	# Start QEMU with debugger
	($(QEMU)   \
	-S -s      \
	-kernel $< \
	$(QEMU_FLAGS) > /dev/null &)
	sleep 1
	wmctrl -xr qemu.Qemu-system-$(QEMU_ARCH) -b add,above

# ****************************
# * Documentation Generation *
# ****************************

.PHONY: docs
docs:
	@echo Generating docs according to the Doxyfile...
	@doxygen ./Doxyfile

.PHONY: todo
todo:
	-@for file in $(ALLFILES:Makefile=); do fgrep -i -H --color=always -e TODO -e FIXME $$file; done; true

# ********************
# * Cleaning Targets *
# ********************

# Clear out objects and BIN
.PHONY: clean
clean:
	@printf "$(COLOR_OK)Cleaning objects...$(COLOR_NONE)\n"
	$(RM) -r $(PRODUCT)/$(KERNEL) $(PRODUCT)/$(SYMBOLS) $(PRODUCT)/$(ISOIMG) $(BUILD)
	@printf "$(COLOR_OK)Cleaning directories...$(COLOR_NONE)\n"
	$(RM) -r $(OBJ_DIRS)
	@printf "$(COLOR_OK)Cleaning libraries...$(COLOR_NONE)\n"
	@for dir in $(LIB_DIRS); do    \
		printf " -   " &&          \
        $(MAKE) -s -C $$dir clean; \
    done
	@printf "$(COLOR_OK)Cleaning complete.$(COLOR_NONE)\n"

.PHONY: clean-vm
clean-vm:
	@echo Removing VM
	@$(VBOX) unregistervm $(VM_NAME) --delete
