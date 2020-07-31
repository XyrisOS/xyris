# Panix Kernel Makefile
# Compiles the kernel source code located in the kernel/ folder.
#
# TODO: Create seperate makefiles as needed and integrate into one makefile
#
# Necessary packages (not including cross compiler)
# brew / apt install qemu-system-i386 grub-pc:i386 xorriso

# Sources and headers
CPP_SRC  = $(shell find kernel/ -name "*.cpp")
ATT_SRC  = $(shell find kernel/ -name "*.s")
HEADERS  = $(shell find sysroot/usr/include/ -name "*.hpp")
SYSROOT  = sysroot

EMU_ARCH = x86_64

# Compilers/Assemblers/Linkers
AS 	= $(shell command -v i686-elf-as 	     || command -v as)
GCC  	= $(shell command -v i686-elf-gcc	     || command -v gcc)
GDB  	= $(shell command -v i686-elf-gdb	     || command -v gdb)
LD   	= $(shell command -v i686-elf-ld 	     || command -v ld)
OBCP 	= $(shell command -v i686-elf-objcopy        || command -v objcopy)
QEMU 	= $(shell command -v qemu-system-$(EMU_ARCH) || echo "Please install qemu")
MKGRUB 	= $(shell command -v grub-mkrescue	     || echo "You're likely on macOS. Please refer to Installing_GRUB_2_on_OS_X on the OSDev Wiki")
VBOX	= $(shell command -v VBoxManage		     || echo "Please install Virtualbox")

# Compiler/Linker flags
# The -lgcc flag is included because it includes helpful functions used
# by GCC that would be ineffective to duplicate.
GCC_FLAGS = 				\
	-m32				\
	-g				\
	-nostartfiles			\
	-nodefaultlibs			\
	-lgcc				\
	-ffreestanding			\
	-fstack-protector-all	\
	-fpermissive			\
	-fno-use-cxa-atexit		\
	-fno-builtin			\
	-fno-rtti			\
	-fno-exceptions			\
	-fno-leading-underscore	        \
	-Wno-write-strings		\
	-std=c++17
AS_FLAGS   = --32
LD_FLAGS   = -melf_i386
KRNL_FLAGS = 					\
	-D__is_kernel 				\
	-I ${SYSROOT}/usr/include/kernel/	
QEMU_FLAGS =                            \
        -m 256M                         \
        -rtc clock=host                 \
        -vga std                        \
        -serial stdio
VM_NAME=panix-box
VBOX_VM_FILE=dist/$(VM_NAME)/$(VM_NAME).vbox

# Linker file
LINKER = kernel/arch/x86/linker.ld

# All objects
OBJ = $(patsubst kernel/%.cpp, obj/%.o, $(CPP_SRC)) 	\
	  $(patsubst kernel/%.s, obj/%.o, $(ATT_SRC))
# Object directories, mirroring source
OBJ_DIRS = $(subst kernel, obj, $(shell find kernel -type d))

# Compile sources to objects
obj/%.o: kernel/%.cpp $(HEADERS)
	$(MAKE) obj_directories
	$(GCC) $(GCC_FLAGS) $(KRNL_FLAGS) -c -o $@ $<

obj/%.o: kernel/%.s
	$(MAKE) obj_directories
	$(AS) $(AS_FLAGS) -o $@ $<

# Link objects into BIN
dist/panix.kernel: $(LINKER) $(OBJ)
	@ mkdir -p dist
	$(LD) $(LD_FLAGS) -T $< -o $@ $(OBJ)
	$(OBCP) --only-keep-debug dist/panix.kernel dist/panix.sym

# Create bootable ISO
dist/panix.iso: dist/panix.kernel
	@ mkdir -p iso/boot/grub
	@ cp $< iso/boot/
	@ cp boot/grub.cfg iso/boot/grub/
	@ $(MKGRUB) -o dist/panix.iso iso
	@ rm -rf iso

# Create object file directories
.PHONY: obj_directories
obj_directories:
	mkdir -p $(OBJ_DIRS)

# Run bootable ISO
.PHONY: run
run: dist/panix.iso
	$(QEMU) 			\
	-drive format=raw,file=$< 	\
	$(QEMU_FLAGS)

$(VBOX_VM_FILE): dist/panix.iso
	$(shell test -f $(VBOX_VM_FILE))
	@ echo The shell said $(.SHELLSTATUS)
ifeq ($(.SHELLSTATUS),"1")
	$(VBOX) createvm --register --name $(VM_NAME) --basefolder $(shell pwd)/dist
	$(VBOX) modifyvm $(VM_NAME)                \
	--memory 256 --ioapic on --cpus 2 --vram 16   \
	--graphicscontroller vboxvga --boot1 disk     \
	--audiocontroller sb16 --uart1 0x3f8 4        \
	--uartmode1 file $(shell pwd)/com1.txt 
	$(VBOX) storagectl $(VM_NAME) --name "DiskDrive" --add ide --bootable on
	$(VBOX) storageattach $(VM_NAME) --storagectl "DiskDrive" --port 1 --device 1 --type dvddrive --medium dist/panix.iso 
else
	@ echo "VBox machine already exists"
endif

.PHONY: virtualbox
virtualbox: $(VBOX_VM_FILE)
	$(VBOX) startvm --putenv --debug $(VM_NAME)

# Open the connection to qemu and load our kernel-object file with symbols
.PHONY: debug
debug: dist/panix.iso
	# Start QEMU with debugger
	($(QEMU) 			\
	-S -s 				\
	-drive format=raw,file=$< 	\
	$(QEMU_FLAGS) &)
	sleep 1
	wmctrl -xr qemu.Qemu-system-$(EMU_ARCH) -b add,above
	# After this start the visual studio debugger
	# gdb dist/panix.kernel

# Install BIN file to local system
install: dist/panix.kernel
	sudo cp $< /boot/panix.kernel

dist: dist/panix.kernel
	@ echo Building VDI image of Panix...
	@ qemu-img convert -f raw -O vdi dist/panix.kernel dist/panix.vdi
	@ echo Done building VDI image of Panix!

	@ echo "\nBuilding VMDK image of Panix..."
	@ qemu-img convert -f raw -O vmdk dist/panix.kernel dist/panix.vmdk
	@ echo Done building VMDK image of Panix!

.PHONY: verify
verify: dist/panix.kernel
	$(shell grub-file --is-x86-multiboot $<)
ifeq ($(.SHELLSTATUS), 1)
	@ echo Kernel does not have valid multiboot!
else
	@ echo Kernel multiboot is valid!
endif

docs:
	@ echo Generating docs according to the Doxyfile...
	@ doxygen ./Doxyfile

# Clear out objects and BIN
.PHONY: clean
clean:
	@ echo Cleaning obj directory...
	@ rm -rf obj
	@ echo Cleaning bin files...
	@ rm -rf dist/*.kernel
	@ rm -rf iso
	@ echo "Done cleaning!"

.PHONY: clean-vm
clean-vm:
	@ echo Removing VM
	@ $(VBOX) unregistervm $(VM_NAME) --delete
