# sudo apt-get install g++ binutils qemu-system-i386 grub-pc:i386 xorriso

# Sources and headers
CPP_SRC  = $(shell find kernel/ -name "*.cpp")
ATT_SRC  = $(shell find kernel/ -name "*.s")
NASM_SRC = $(shell find kernel/ -name "*.nasm")
HEADERS  = $(shell find sysroot/usr/include/ -name "*.hpp")
SYSROOT  = sysroot

# Compilers/Assemblers/Linkers
NASM = $(shell command -v nasm 			|| echo "Please install nasm")
AS	 = $(shell command -v i686-elf-as 	|| as)
GCC  = $(shell command -v i686-elf-gcc 	|| gcc)
GDB  = $(shell command -v i686-elf-gdb 	|| gdb)
LD   = $(shell command -v i686-elf-ld 	|| ld)
QEMU = qemu-system-i386

# Compilers/Assemblers/Linkers for Automation
STD_AS  = as
STD_GCC = gcc
STD_GDB = gdb
STD_LD  = ld

# Compiler/Linker flags
GCC_FLAGS = 					\
	-m32 						\
	-g							\
	-nostdlib					\
	-nostartfiles				\
	-nodefaultlibs				\
	-ffreestanding				\
	-fno-use-cxa-atexit			\
	-fno-builtin				\
	-fno-rtti					\
	-fno-exceptions				\
	-fno-leading-underscore		\
	-fno-stack-protector		\
	-Wno-write-strings			\
	-std=c++17
AS_FLAGS   = --32
NASM_FLAGS = -f elf
LD_FLAGS   = -melf_i386
KRNL_FLAGS = 							\
	-D__is_kernel 						\
	-I ${SYSROOT}/usr/include/kernel/	

# Linker file
LINKER = kernel/arch/x86/linker.ld

# All objects
OBJ = $(patsubst kernel/%.cpp, obj/%.o, $(CPP_SRC)) 	\
	  $(patsubst kernel/%.s, obj/%.o, $(ATT_SRC)) 		\
	  $(patsubst kernel/%.nasm, obj/%.o, $(NASM_SRC))
# Object directories, mirroring source
OBJ_DIRS = $(subst kernel, obj, $(shell find kernel -type d))

# Compile sources to objects
obj/%.o: kernel/%.cpp $(HEADERS)
	$(MAKE) obj_directories
	$(GCC) $(GCC_FLAGS) $(KRNL_FLAGS) -c -o $@ $<

obj/%.o: kernel/%.s
	$(MAKE) obj_directories
	$(AS) $(AS_FLAGS) -o $@ $<

obj/%.o: kernel/%.nasm
	$(MAKE) obj_directories
	$(NASM) $(NASM_FLAGS) -o $@ $<

# Link objects into BIN
dist/panix.kernel: $(LINKER) $(OBJ)
	@ mkdir -p dist
	$(LD) $(LD_FLAGS) -T $< -o $@ $(OBJ)
	objcopy --only-keep-debug dist/panix.kernel dist/panix.sym

# Create bootable ISO
dist/panix.iso: dist/panix.kernel
	@ mkdir -p iso/boot/grub
	@ cp $< iso/boot/
	@ cp boot/grub.cfg iso/boot/grub/
	@ grub-mkrescue -o dist/panix.iso iso
	@ rm -rf iso

# Create object file directories
.PHONY: 
	obj_directories
obj_directories:
	mkdir -p $(OBJ_DIRS)

MULTIBOOT = $(shell grub-file --is-x86-multiboot dist/panix.kernel)
verify:
ifeq ($(.SHELLSTATUS), 1)
	@ echo Kernel does not have valid multiboot!
else
	@ echo Kernel multiboot is valid!
endif

# Run bootable ISO
run: dist/panix.iso
	$(QEMU) 					\
	-drive format=raw,file=$< 	\
	-m 128M						\
	-soundhw pcspk 				\
	-rtc clock=host 			\
	-vga std -m 256M 			\
	-serial stdio				\
	-d cpu_reset

virtualbox:
	VBoxManage startvm --putenv --debug "Panix"

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

# Open the connection to qemu and load our kernel-object file with symbols
debug: dist/panix.iso
	# Start QEMU with debugger
	$(QEMU) 					\
	-S -s 						\
	-drive format=raw,file=$< 	\
	-m 128M						\
	-soundhw pcspk 				\
	-rtc clock=host 			\
	-vga std 					\
	-m 256M 					\
	-serial stdio				\
	-d cpu_reset				
	# After this start the visual studio debugger
	# gdb dist/panix.kernel

docs:
	@ echo Generating docs according to the Doxyfile...
	@ doxygen ./Doxyfile

# Clear out objects and BIN
clean:
	@ echo Cleaning obj directory...
	@ rm -rf obj
	@ echo Cleaning bin files...
	@ rm -rf dist/*.kernel
	@ rm -rf iso
	@ echo "Done cleaning!"