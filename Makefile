#  ____             _
# |  _ \ __ _ _ __ (_)_  __
# | |_) / _` | '_ \| \ \/ /
# |  __/ (_| | | | | |>  <
# |_|   \__,_|_| |_|_/_/\_\
#
# Compiles the kernel source code located in the kernel folder.

# Designed by Keeton Feavel & Micah Switzer
# Copyright the Panix Contributors (c) 2019

export PROJ_NAME = panix

# Makefile flags
# prevent make from showing "entering/leaving directory" messages
export MAKEFLAGS   += --no-print-directory
# Panix Version
export GIT_COMMIT := "$(shell git describe --abbrev=8 --dirty --always --tags)"
export VER_MAJOR := "0"
export VER_MINOR := "4"
export VER_PATCH := "0"
export VER_NAME := "Phoenix"

# ******************************
# * Compiler Output Formatting *
# ******************************

export COLOR_COM  = \033[0;34m
export COLOR_OK   = \033[0;32m
export COLOR_INFO = \033[0;93m
export COLOR_NONE = \033[m

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

# *****************************
# * Source Code & Directories *
# *****************************

# Directories & files
BUILD   = obj
LIBRARY = libs
KERNEL  = kernel
LIMINE  = ./thirdparty/limine
ISOIMG  = $(PROJ_NAME).iso
IMGIMG  = $(PROJ_NAME).img
SYMBOLS = $(KERNEL).sym
PRODUCT = dist
TESTS   = tests
IMGTYPE ?= img
RUNIMG  := $(PROJ_NAME).$(IMGTYPE)

# Libraries
# Okay, time to rant. For *whatever* reason, when I compile and install my
# i686-elf cross compiler, libgcc can never be found. It doesn't matter if
# I'm running macOS, Arch, etc. It always has this issue, so, I decided to
# solve it by just having GCC tell me where it is and then linking against
# that absolute path directly. That way I never have to deal with it again
export LIB_DIRS := $(shell find $(LIBRARY) -mindepth 1 -maxdepth 1 -type d)
export LIB_GCC  := $(shell $(CC) -print-libgcc-file-name)

# *******************
# * Toolchain Flags *
# *******************

# Warning flags
# (Disable unused functions warning)
export WARNINGS :=          \
	-Wall                   \
	-Werror                 \
	-Wextra                 \
	-Wundef                 \
	-Winline                \
	-Wshadow                \
	-Wformat=2              \
	-Wcast-align            \
	-Wno-long-long          \
	-Wpointer-arith         \
	-Wwrite-strings         \
	-Wredundant-decls       \
	-Wdouble-promotion      \
	-Wno-unused-function    \
	-Wmissing-declarations
# Add this back in soon when
# we can fix the last conversion
# issue.
#	-Wconversion
# Flags to be added later
#   -Wconversion
# C only warnings
export CWARNINGS :=         \
	-Wnested-externs        \
	-Wstrict-prototypes     \
	-Wmissing-prototypes    \
# C flags (include directory)
export CFLAGS :=            \
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
export CXXFLAGS :=          \
	-fpermissive            \
	-fno-rtti               \
	-fno-exceptions         \
	-fno-use-cxa-atexit     \
	-std=c++17              \
	${PANIX_CXXFLAGS}       \
# C / C++ pre-processor flags
export CPPFLAGS :=                \
	${PANIX_CPPFLAGS}             \
	-D COMMIT=\"$(GIT_COMMIT)\"   \
	-D VER_MAJOR=\"$(VER_MAJOR)\" \
	-D VER_MINOR=\"$(VER_MINOR)\" \
	-D VER_PATCH=\"$(VER_PATCH)\" \
	-D VER_NAME=\"$(VER_NAME)\"
# Assembler flags
export ASFLAGS :=           \
	${PANIX_ASFLAGS}        \
	--32
# Linker flags
export LDFLAGS :=           \
	${PANIX_LDFLAGS}        \
	$(LIB_GCC)              \
	-m elf_i386             \
	-L.

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

# Kernel (Linked With Libraries)
.PHONY: $(KERNEL)
$(KERNEL):
	@printf "$(COLOR_INFO)Making Libs$(COLOR_NONE)\n"
	@for dir in $(LIB_DIRS); do        \
        $(MAKE) -C $$dir $(PROJ_NAME); \
    done
	@printf "$(COLOR_INFO)Making Kernel$(COLOR_NONE)\n"
	@$(MAKE) -C $(KERNEL) $(KERNEL)
	@printf "$(COLOR_INFO)Done!$(COLOR_NONE)\n"

$(PRODUCT)/$(KERNEL): $(KERNEL)

# **********************
# * Bootloader Targets *
# **********************

$(LIMINE)/limine-install:
	@printf "$(COLOR_INFO)Making Limine Bootloader$(COLOR_NONE)\n"
	@$(MAKE) -C $(LIMINE) limine-install
	@printf "$(COLOR_INFO)Done!$(COLOR_NONE)\n"

# *********************
# * Kernel Unit Tests *
# *********************

.PHONY: $(TESTS)
$(TESTS):
	@$(MAKE) -C $(TESTS)

.PHONY: test
test:
	@$(MAKE) -C $(TESTS) test

# ********************************
# * Kernel Distribution Creation *
# ********************************

# Create bootable ISO
$(PRODUCT)/$(ISOIMG): $(PRODUCT)/$(KERNEL)
	@mkdir -p iso/boot/grub
	@cp $(PRODUCT)/$(KERNEL) iso/boot/
	@cp boot/grub.cfg iso/boot/grub/grub.cfg
	@$(MKGRUB) -o $@ iso
	@rm -rf iso

# Create a bootable IMG
$(PRODUCT)/$(IMGIMG): $(PRODUCT)/$(KERNEL) $(LIMINE)/limine-install
	@printf "$(COLOR_INFO)Making Limine boot image$(COLOR_NONE)\n"
	@rm -f $@
	@dd if=/dev/zero bs=1M count=0 seek=64 of=$@ 2> /dev/null
	@parted -s $@ mklabel msdos
	@parted -s $@ mkpart primary 1 100%
	@parted -s $@ set 1 boot on # Workaround for buggy BIOSes
	@echfs-utils -m -p0 $@ quick-format 32768
	@echfs-utils -m -p0 $@ import boot/limine.cfg limine.cfg
	@echfs-utils -m -p0 $@ import $< kernel
	#@echfs-utils -m -p0 $@ import boot/bg.bmp bg.bmp
	@$(LIMINE)/limine-install $(LIMINE)/limine.bin $@

# Create a bootable image (either img or iso)
.PHONY: $(PRODUCT)
$(PRODUCT): $(PRODUCT)/$(RUNIMG)

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
VM_NAME = $(PROJ_NAME)-box
VBOX_VM_FILE = $(PRODUCT)/$(VM_NAME)/$(VM_NAME).vbox
# VM executable locations
VBOX = $(shell command -v VBoxManage)
QEMU = $(shell command -v qemu-system-$(QEMU_ARCH))

# ***************************
# * Virtual Machine Testing *
# ***************************

# Run Panix in QEMU
.PHONY: run
run: $(PRODUCT)/$(RUNIMG)
	$(QEMU)                           \
	-drive file=$<,index=0,media=disk,format=raw \
	$(QEMU_FLAGS)

# Open the connection to qemu and load our kernel-object file with symbols
.PHONY: run-debug
run-debug: $(PRODUCT)/$(RUNIMG)
	# Start QEMU with debugger
	($(QEMU)   \
	-S -s      \
	-drive file=$<,index=0,media=disk,format=raw \
	$(QEMU_FLAGS) > /dev/null &)
	sleep 1
	wmctrl -xr qemu.Qemu-system-$(QEMU_ARCH) -b add,above

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
	$(RM) -r $(PRODUCT)/$(KERNEL) $(PRODUCT)/$(SYMBOLS) $(PRODUCT)/$(ISOIMG)
	@printf "$(COLOR_OK)Cleaning directories...$(COLOR_NONE)\n"
	$(RM) -r $(BUILD)
	@printf "$(COLOR_OK)Cleaning libraries...$(COLOR_NONE)\n"
	@for dir in $(LIB_DIRS); do \
	    printf " -   " &&       \
        $(MAKE) -C $$dir clean; \
    done
	@printf "$(COLOR_OK)Cleaning bootloader...$(COLOR_NONE)\n"
	@$(MAKE) -C $(LIMINE) clean
	@printf "$(COLOR_OK)Cleaning complete.$(COLOR_NONE)\n"

.PHONY: clean-tests
clean-tests:
	@$(MAKE) -C $(TESTS) clean

.PHONY: clean-vm
clean-vm:
	@echo Removing VM
	@$(VBOX) unregistervm $(VM_NAME) --delete
