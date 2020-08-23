# Panix Kernel Makefile
# Compiles the kernel source code located in the kernel/ folder.
#
# TODO: Create seperate makefiles as needed and integrate into one makefile
#

.DEFAULT_GOAL := i686
GIT_VERSION   := "$(shell git describe --abbrev=4 --dirty --always --tags)"

# *****************************
# * Various Source Code Flags *
# *****************************

SYSROOT	= sysroot
INCLUDE = $(SYSROOT)/usr/include
CPP_SRC = $(shell find kernel -name "*.cpp")
ATT_SRC = $(shell find kernel -name "*.s")
HEADERS = $(shell find $(INCLUDE) -name "*.hpp" -name "*.h")
# Grub mkrescue is included here since
# I'm not sure where else it would go
MKGRUB 	= $(shell command -v grub-mkrescue)

# *********************************
# * Various Virtual Machine Flags *
# *********************************

# QEMU flags
QEMU_FLAGS =		\
    -m 4G			\
    -rtc clock=host	\
    -vga std		\
    -serial stdio
QEMU_ARCH = x86_64
# Virtualbox flags
VM_NAME	= panix-box
VBOX_VM_FILE=dist/$(VM_NAME)/$(VM_NAME).vbox
# VM executable locations
VBOX = $(shell command -v VBoxManage)
QEMU = $(shell command -v qemu-system-$(QEMU_ARCH))

# ********************************
# * Common Kernel Compiler Flags *
# ********************************

# Kernel define flags
KRNL_FLAGS = \
	-I ${SYSROOT}/usr/include/kernel/ \
	-D VERSION=\"$(GIT_VERSION)\"

# **********************************
# * 32-Bit i686 Architecture Flags *
# **********************************

# Compilers/Assemblers/Linkers
AS_32 	= $(shell command -v i686-elf-as)
GCC_32  = $(shell command -v i686-elf-gcc)
LD_32   = $(shell command -v i686-elf-ld)
OBCP_32 = $(shell command -v i686-elf-objcopy)
# The -lgcc flag is included because it includes helpful functions used
# by GCC that would be ineffective to duplicate.
GCC_FLAGS_32 =              \
	-m32                    \
	-g                      \
	-nostartfiles           \
	-nodefaultlibs          \
	-lgcc                   \
	-ffreestanding          \
	-fstack-protector-all   \
	-fpermissive            \
	-fno-use-cxa-atexit     \
	-fno-builtin            \
	-fno-rtti               \
	-fno-exceptions         \
	-fno-leading-underscore \
	-fno-omit-frame-pointer \
	-Wno-write-strings      \
	-std=c++2a
# i686 Assembler flags
AS_FLAGS_32 = --32
# i686 Linker flags
LD_FLAGS_32 = -m elf_i386
LD_SCRIPT_32 = kernel/arch/i386/linker.ld

# ************************************
# * 64-Bit x86_64 Architecture Flags *
# ************************************

# Compilers/Assemblers/Linkers
AS_64 	= $(shell command -v x86_64-elf-as)
GCC_64  = $(shell command -v x86_64-elf-gcc)
LD_64   = $(shell command -v x86_64-elf-ld)
OBCP_64 = $(shell command -v x86_64-elf-objcopy)
# The -lgcc flag is included because it includes helpful functions used
# by GCC that would be ineffective to duplicate.
GCC_FLAGS_64 =              \
	-m64                    \
	-g                      \
	-nostartfiles           \
	-nodefaultlibs          \
	-lgcc                   \
	-ffreestanding          \
	-fstack-protector-all   \
	-fpermissive            \
	-fno-use-cxa-atexit     \
	-fno-builtin            \
	-fno-rtti               \
	-fno-exceptions         \
	-fno-leading-underscore \
	-Wno-write-strings      \
	-std=c++20
# i686 Assembler flags
AS_FLAGS_64 = --64
# i686 Linker flags
LD_FLAGS_64 = -m elf_x86_64
LD_SCRIPT_64 = kernel/arch/i386/linker.ld

# ***********************************
# * Source Code Compilation Targets *
# ***********************************

# All objects
OBJ = $(patsubst kernel/%.cpp, obj/%.o, $(CPP_SRC))	\
	  $(patsubst kernel/%.s, obj/%.o, $(ATT_SRC))
# Object directories, mirroring source
OBJ_DIRS = $(subst kernel, obj, $(shell find kernel -type d))

# Create object file directories
.PHONY: mkdir_obj_dirs
mkdir_obj_dirs:
	mkdir -p $(OBJ_DIRS)

# C++ source -> object
obj/%.o: kernel/%.cpp $(HEADERS)
	$(MAKE) mkdir_obj_dirs
	$(GCC) $(GCC_FLAGS) $(KRNL_FLAGS) -c -o $@ $<

# Assembly source -> object
obj/%.o: kernel/%.s
	$(MAKE) mkdir_obj_dirs
	$(AS) $(AS_FLAGS) -o $@ $<

# Kernel object
dist/kernel: $(OBJ)
	@ mkdir -p dist
	$(LD) $(LD_FLAGS) -T $(LD_SCRIPT) -o $@ $(OBJ)
	$(OBCP) --only-keep-debug dist/kernel dist/panix.sym

# *****************************
# * Architecture Make Targets *
# *****************************

# i686 Architecture
i686: GCC = $(GCC_32)
i686: GCC_FLAGS = $(GCC_FLAGS_32)
i686: AS = $(AS_32)
i686: AS_FLAGS = $(AS_FLAGS_32)
i686: LD = $(LD_32)
i686: LD_FLAGS = $(LD_FLAGS_32)
i686: LD_SCRIPT = $(LD_SCRIPT_32)
i686: OBCP = $(OBCP_32)
i686: dist/kernel

# amd64 Architecture
amd64: GCC = $(GCC_64)
amd64: GCC_FLAGS = $(GCC_FLAGS_64)
amd64: AS = $(AS_64)
amd64: AS_FLAGS = $(AS_FLAGS_64)
amd64: dist/kernel

# ********************************
# * Kernel Distribution Creation *
# ********************************

# Create bootable ISO
.PHONY: iso
iso32: i686
	@ mkdir -p iso/boot/grub
	@ cp dist/kernel iso/boot/
	@ cp boot/grub.cfg iso/boot/grub/grub.cfg
	@ $(MKGRUB) -o dist/panix.iso iso
	@ rm -rf iso

.PHONY: vdi32
vdi32: i686
	@ echo Building VDI image of Panix...
	@ qemu-img convert -f raw -O vdi dist/kernel dist/panix.vdi
	@ echo Done building VDI image of Panix!

.PHONY: vmdk32
vmdk32: i686
	@ echo "\nBuilding VMDK image of Panix..."
	@ qemu-img convert -f raw -O vmdk dist/kernel dist/panix.vmdk
	@ echo Done building VMDK image of Panix!

# ***************************
# * Virtual Machine Testing *
# ***************************

# Run Panix in QEMU
.PHONY: run
run: i686
	$(QEMU)                     \
	-kernel dist/kernel   \
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
	$(VBOX) storageattach $(VM_NAME) --storagectl "DiskDrive" --port 1 --device 1 --type dvddrive --medium dist/panix32.iso

.PHONY: vbox-create
vbox: vbox-create
	$(VBOX) startvm --putenv --debug $(VM_NAME)

# Open the connection to qemu and load our kernel-object file with symbols
.PHONY: debug
debug: dist/kernel
	# Start QEMU with debugger
	($(QEMU)         \
	-S -s            \
	-kernel $<       \
	$(QEMU_FLAGS) &)
	sleep 1
	wmctrl -xr qemu.Qemu-system-$(QEMU_ARCH) -b add,above
	# After this start the visual studio debugger
	# gdb dist/kernel

# ************************************
# * Doxygen Documentation Generation *
# ************************************

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
