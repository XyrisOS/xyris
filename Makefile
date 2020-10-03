# Panix Kernel Makefile
# Compiles the kernel source code located in the kernel/ folder.
#
# TODO: Create seperate makefiles as needed and integrate into one makefile
#

.DEFAULT_GOAL := dist/kernel
GIT_VERSION   := "$(shell git describe --abbrev=8 --dirty --always --tags)"

# *****************************
# * Various Source Code Flags *
# *****************************

SYSROOT	= sysroot
INCLUDE = $(SYSROOT)/usr/include
ATT_SRC = $(shell find kernel -name "*.s")
NASM_SRC = $(shell find kernel -name "*.S")
CPP_SRC = $(shell find kernel -name "*.cpp")
HEADERS = $(shell find $(INCLUDE) -name "*.hpp" -name "*.h")

# *********************************
# * Various Virtual Machine Flags *
# *********************************

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

# **********************************
# * 32-Bit i686 Architecture Flags *
# **********************************

# Compilers/Assemblers/Linkers
AS      = $(shell command -v i686-elf-as)
NASM    = $(shell command -v llc)
CXX     = $(shell command -v clang++)
LD      = $(shell command -v ld.lld)
OBJCP   = $(shell command -v llvm-objcopy)
MKGRUB  = $(shell command -v grub-mkrescue)
# C / C++ flags (include directory)
CFLAGS =                   \
	-I ${SYSROOT}/usr/include/kernel/
# C++ only flags (-lgcc flag is used b/c it has helpful functions)
# Flags explained:
#
# -Wno-unused-function
# We need to ignore unused functions because we may use
# them at a later time. For example, paging disable.
#
CXXFLAGS =                  \
	-g                      \
	-m32                    \
	-target i386-none-elf   \
	-ffreestanding          \
	-fstack-protector-all   \
	-fpermissive            \
	-nodefaultlibs          \
	-fno-use-cxa-atexit     \
	-fno-builtin            \
	-fno-rtti               \
	-fno-exceptions         \
	-fno-omit-frame-pointer \
	-Wall                   \
	-Werror                 \
	-Wno-unused-function    \
	-std=c++17
# C / C++ pre-processor flags
CPP_FLAGS =                 \
	-D VERSION=\"$(GIT_VERSION)\"
# Assembler flags
AS_FLAGS = --32
# Linker flags
LD_FLAGS = -m elf_i386 \
           -T kernel/arch/i386/linker.ld

# ***********************************
# * Source Code Compilation Targets *
# ***********************************

# All objects
OBJ = $(patsubst kernel/%.cpp, obj/%.o, $(CPP_SRC)) \
	  $(patsubst kernel/%.s, obj/%.o, $(ATT_SRC)) \
	  $(patsubst kernel/%.S, obj/%.o, $(NASM_SRC))
# Object directories, mirroring source
OBJ_DIRS = $(subst kernel, obj, $(shell find kernel -type d))

# Create object file directories
.PHONY: mkdir_obj_dirs
mkdir_obj_dirs:
	mkdir -p $(OBJ_DIRS)

# C++ source -> object
obj/%.o: kernel/%.cpp $(HEADERS)
	$(MAKE) mkdir_obj_dirs
	$(CXX) $(CPP_FLAGS) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<

# Assembly source -> object
obj/%.o: kernel/%.s
	$(MAKE) mkdir_obj_dirs
	$(AS) $(AS_FLAGS) -o $@ $<

obj/%.o: kernel/%.S
	$(MAKE) mkdir_obj_dirs
	$(NASM) -felf -o $@ $<

# Kernel object
dist/kernel: $(OBJ)
	@ mkdir -p dist
	$(LD) $(LD_FLAGS) -o $@ $(OBJ)
	$(OBJCP) --only-keep-debug dist/kernel dist/panix.sym

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
