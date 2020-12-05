#  ____             _        _  __                    _
# |  _ \ __ _ _ __ (_)_  __ | |/ /___ _ __ _ __   ___| |
# | |_) / _` | '_ \| \ \/ / | ' // _ \ '__| '_ \ / _ \ |
# |  __/ (_| | | | | |>  <  | . \  __/ |  | | | |  __/ |
# |_|   \__,_|_| |_|_/_/\_\ |_|\_\___|_|  |_| |_|\___|_|
#
# Compiles the kernel source code located in the kernel folder.

# Designed by Keeton Feavel & Micah Switzer
# Copyright the Panix Contributors (c) 2019

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
ISOIMG  = panix.iso
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
LIB_DIRS = $(shell find $(LIBRARY) -type d)
LIBS_A   = $(shell find $(LIBRARY) -type f -name "*.a")
LIBS     = $(addprefix -l:, $(LIBS_A))

# *******************
# * i686 Toolchains *
# *******************

# Compilers/Assemblers/Linkers
NASM    = $(shell command -v nasm)
AS      = $(shell command -v i686-elf-as)
CC      = $(shell command -v i686-elf-gcc)
CXX     = $(shell command -v i686-elf-g++)
LD      = $(shell command -v i686-elf-ld)
OBJCP   = $(shell command -v i686-elf-objcopy)
MKGRUB  = $(shell command -v grub-mkrescue)

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
# Common (C & C++) flags
FLAGS :=                    \
	-m32                    \
	-nostdlib               \
	-nodefaultlibs          \
	-fpermissive            \
	-ffreestanding          \
	-fstack-protector-all   \
	-fno-rtti               \
	-fno-builtin            \
	-fno-exceptions         \
	-fno-use-cxa-atexit     \
	-fno-omit-frame-pointer
# C flags (include directory)
CFLAGS :=           \
	${FLAGS}        \
	${PANIX_CFLAGS} \
	${WARNINGS}     \
	$(CWARNINGS)    \
	-std=c17
# C++ flags
CXXFLAGS :=           \
	${FLAGS}          \
	${PANIX_CXXFLAGS} \
	${WARNINGS}       \
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
LDFLAGS :=                        \
	${PANIX_LDFLAGS}              \
	-m elf_i386                   \
	-T kernel/arch/i386/linker.ld \
	-L $(LIBRARY)                 \
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
release: $(PRODUCT)/$(KERNEL)

# Debug build
debug: CXXFLAGS += -DDEBUG -g
debug: CFLAGS += -DDEBUG -g
debug: $(PRODUCT)/$(KERNEL)

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
# Dependency files
DEP = $(OBJ_CPP:%.o=%.d) $(OBJ_C:%.o=%.d)
# All files (source, header, etc.)
ALLFILES = $(ATT_SRC) $(NASM_SRC) $(C_SRC) $(CPP_SRC) $(HEADERS)
# Include all .d files
-include $(DEP)

# Create object file directories
.PHONY: mkdir_obj_dirs
mkdir_obj_dirs:
	@mkdir -p $(OBJ_DIRS)

# C source -> object
$(BUILD)/%.o: $(KERNEL)/%.c $(HEADERS)
	@$(MAKE) mkdir_obj_dirs
	@$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -c -o $@ $<
	@printf "$(COLOR_COM)(CC)$(COLOR_NONE)\t$@\n"
# C++ source -> object
$(BUILD)/%.o: $(KERNEL)/%.cpp $(HEADERS)
	@$(MAKE) mkdir_obj_dirs
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -c -o $@ $<
	@printf "$(COLOR_COM)(CXX)$(COLOR_NONE)\t$@\n"
# GAS assembly -> object
$(BUILD)/%.o: $(KERNEL)/%.s
	@$(MAKE) mkdir_obj_dirs
	@$(AS) $(ASFLAGS) -o $@ $<
	@printf "$(COLOR_COM)(AS)$(COLOR_NONE)\t$@\n"
# NASM assembly -> object
$(BUILD)/%.o: $(KERNEL)/%.S
	@$(MAKE) mkdir_obj_dirs
	@$(NASM) -f elf32 -o $@ $<
	@printf "$(COLOR_COM)(NASM)$(COLOR_NONE)\t$@\n"
# Kernel object
$(PRODUCT)/$(KERNEL): $(LIBRARY) $(OBJ)
	@mkdir -p $(PRODUCT)
	@$(LD) -o $@ $(OBJ) $(LDFLAGS)
	@$(OBJCP) --only-keep-debug $(PRODUCT)/$(KERNEL) $(PRODUCT)/$(SYMBOLS)

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
VM_NAME	= panix-box
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
	$(RM) $(PRODUCT)/$(KERNEL) $(PRODUCT)/$(SYMBOLS) $(OBJ) $(DEP)
	@printf "$(COLOR_OK)Cleaning directories...$(COLOR_NONE)\n"
	$(RM) -r $(OBJ_DIRS)
	@printf "$(COLOR_OK)Cleaning complete.$(COLOR_NONE)\n"

.PHONY: clean-vm
clean-vm:
	@echo Removing VM
	@$(VBOX) unregistervm $(VM_NAME) --delete
