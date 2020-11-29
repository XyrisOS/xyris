# Panix Kernel Makefile
# Compiles the kernel source code located in the kernel/ folder.
#
# TODO: Create seperate makefiles as needed and integrate into one makefile
#

.DEFAULT_GOAL := release
GIT_VERSION   := "$(shell git describe --abbrev=8 --dirty --always --tags)"

# *****************************
# * Source Code & Directories *
# *****************************

# Directories
BUILD   = obj
LIBRARY = libs
KERNEL  = kernel
PRODUCT = dist
SYSROOT	= sysroot
INCLUDE = $(SYSROOT)/usr/include

# Source code
ATT_SRC  = $(shell find $(KERNEL) $(LIBRARY) -name "*.s")
NASM_SRC = $(shell find $(KERNEL) $(LIBRARY) -name "*.S")
C_SRC    = $(shell find $(KERNEL) $(LIBRARY) -name "*.c")
CPP_SRC  = $(shell find $(KERNEL) $(LIBRARY) -name "*.cpp")
HEADERS  = $(shell find $(INCLUDE) $(LIBRARY) -name "*.hpp" -or -name "*.h")

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
	-Wextra              \
	-Winline             \
	-Wshadow             \
	-Wconversion         \
	-Wcast-align         \
	-Wno-long-long       \
	-Wpointer-arith      \
	-Wwrite-strings      \
	-Wredundant-decls    \
	-Wno-unused-function \
	-Wmissing-declarations
# TODO: Include -Werror once we
# Fix all of the warnings
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
	-fno-omit-frame-pointer \
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
	-lgcc

# ************************
# * Kernel Build Targets *
# ************************

# Debug build
debug: CXXFLAGS += -DDEBUG -g
debug: CFLAGS += -DDEBUG -g
debug: dist/kernel

# Release build
release: CXXFLAGS += -O3 -mno-avx
release: CFLAGS += -O3 -mno-avx
release: dist/kernel

# *************************
# * Kernel Source Objects *
# *************************

# All objects
OBJ = $(patsubst $(KERNEL)/%.cpp, $(BUILD)/%.o, $(CPP_SRC)) \
	  $(patsubst $(KERNEL)/%.c,   $(BUILD)/%.o, $(C_SRC))   \
	  $(patsubst $(KERNEL)/%.s,   $(BUILD)/%.o, $(ATT_SRC)) \
	  $(patsubst $(KERNEL)/%.S,   $(BUILD)/%.o, $(NASM_SRC))
# Object directories, mirroring source
OBJ_DIRS = $(subst $(KERNEL), $(BUILD), $(shell find $(KERNEL) -type d))

# Create object file directories
.PHONY: mkdir_obj_dirs
mkdir_obj_dirs:
	mkdir -p $(OBJ_DIRS)

# C source -> object
$(BUILD)/%.o: $(KERNEL)/%.c $(HEADERS)
	$(MAKE) mkdir_obj_dirs
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<
# C++ source -> object
$(BUILD)/%.o: $(KERNEL)/%.cpp $(HEADERS)
	$(MAKE) mkdir_obj_dirs
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
# GAS assembly -> object
$(BUILD)/%.o: $(KERNEL)/%.s
	$(MAKE) mkdir_obj_dirs
	$(AS) $(ASFLAGS) -o $@ $<
# NASM assembly -> object
$(BUILD)/%.o: $(KERNEL)/%.S
	$(MAKE) mkdir_obj_dirs
	$(NASM) -f elf32 -o $@ $<

# Kernel object
$(PRODUCT)/kernel: $(OBJ)
	@ mkdir -p dist
	$(LD) -o $@ $(OBJ) $(LDFLAGS)
	$(OBJCP) --only-keep-debug $(PRODUCT)/kernel $(PRODUCT)/panix.sym

# ********************************
# * Kernel Distribution Creation *
# ********************************

# Create bootable ISO
.PHONY: iso
iso: dist/kernel
	@ mkdir -p iso/boot/grub
	@ cp dist/kernel iso/boot/
	@ cp boot/grub.cfg iso/boot/grub/grub.cfg
	@ $(MKGRUB) -o dist/panix.iso iso
	@ rm -rf iso

.PHONY: vdi
vdi: dist/kernel
	@ echo Building VDI image of Panix...
	@ qemu-img convert -f raw -O vdi dist/kernel dist/panix.vdi
	@ echo Done building VDI image of Panix!

.PHONY: vmdk
vmdk: dist/kernel
	@ echo "\nBuilding VMDK image of Panix..."
	@ qemu-img convert -f raw -O vmdk dist/kernel dist/panix.vmdk
	@ echo Done building VMDK image of Panix!

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
VBOX_VM_FILE=dist/$(VM_NAME)/$(VM_NAME).vbox
# VM executable locations
VBOX = $(shell command -v VBoxManage)
QEMU = $(shell command -v qemu-system-$(QEMU_ARCH))

# ***************************
# * Virtual Machine Testing *
# ***************************

# Run Panix in QEMU
.PHONY: run
run: dist/kernel
	$(QEMU)             \
	-kernel dist/kernel \
	$(QEMU_FLAGS)

# Create Virtualbox VM
.PHONY: vbox-create
vbox-create: dist/panix.iso
	$(VBOX) createvm --register --name $(VM_NAME) --basefolder $(shell pwd)/dist
	$(VBOX) modifyvm $(VM_NAME)                 \
	--memory 256 --ioapic on --cpus 2 --vram 16 \
	--graphicscontroller vboxvga --boot1 disk   \
	--audiocontroller sb16 --uart1 0x3f8 4      \
	--uartmode1 file $(shell pwd)/com1.txt
	$(VBOX) storagectl $(VM_NAME) --name "DiskDrive" --add ide --bootable on
	$(VBOX) storageattach $(VM_NAME) --storagectl "DiskDrive" --port 1 --device 1 --type dvddrive --medium dist/panix.iso

.PHONY: vbox-create
vbox: vbox-create
	$(VBOX) startvm --putenv --debug $(VM_NAME)

# Open the connection to qemu and load our kernel-object file with symbols
.PHONY: debugger
debugger: dist/kernel
	# Start QEMU with debugger
	($(QEMU)   \
	-S -s      \
	-kernel $< \
	$(QEMU_FLAGS) > /dev/null &)
	sleep 1
	wmctrl -xr qemu.Qemu-system-$(QEMU_ARCH) -b add,above
	# After this start the visual studio debugger
	# gdb dist/kernel

# ****************************
# * Documentation Generation *
# ****************************

.PHONY: docs
docs:
	@ echo Generating docs according to the Doxyfile...
	@ doxygen ./Doxyfile

# ********************
# * Cleaning Targets *
# ********************

# Clear out objects and BIN
.PHONY: clean
clean:
	@ echo Cleaning obj directory...
	@ rm -rf obj
	@ echo Cleaning bin files...
	@ rm -rf dist/*
	@ echo "Done cleaning!"

.PHONY: clean-vm
clean-vm:
	@ echo Removing VM
	@ $(VBOX) unregistervm $(VM_NAME) --delete
