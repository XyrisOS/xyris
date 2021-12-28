# __  __          _
# \ \/ /   _ _ __(_)___
#  \  / | | | '__| / __|
#  /  \ |_| | |  | \__ \
# /_/\_\__, |_|  |_|___/
#      |___/
#
# Compiles the kernel source code located in the kernel folder.

# Designed by Keeton Feavel & Micah Switzer
# Copyright the Xyris Contributors (c) 2019
import os

env = Environment(
    tools=[
        'default',
        'echfs',
    ],
    toolpath=[
        'Scones',
    ],
    ENV={
        'PATH': os.environ['PATH'],
    },
    BUILD_DIR='#Build/$ARCH/$MODE',
    INSTALL_DIR='#Distribution/$ARCH/$MODE',
    REPO_URL='https://github.com/XyrisOS/xyris',
    GIT_COMMIT='TODO',
    VERSION_ID=(0, 5, 0),
    VERSION_NAME='Pheonix',

    # *******************
    # * Toolchain Flags *
    # *******************

    # C only warnings
    CWARNINGS=[
        '-Wnested-externs',
        '-Wstrict-prototypes',
        '-Wmissing-prototypes',
    ],
    # C/C++ warnings
    CCWARNINGS=[
	    '-Wall',
	    '-Werror',
	    '-Wextra',
	    '-Wundef',
	    '-Winline',
	    '-Wshadow',
	    '-Wformat=2',
	    '-Wcast-align',
	    '-Wno-long-long',
	    '-Wpointer-arith',
	    '-Wwrite-strings',
	    '-Wredundant-decls',
	    '-Wdouble-promotion',
	    '-Wno-unused-function',
	    '-Wmissing-declarations',
    ],
    # C only flags
    CFLAGS=[
        '${CWARNINGS}'
    ],
    # C/C++ flags
    CCFLAGS=[
        '-nostdlib',
        '-nodefaultlibs',
        '-ffreestanding',
        '-fstack-protector-all',
        '-fno-builtin',
        '-fno-omit-frame-pointer',
        '${CCWARNINGS}'
    ],
    # C++ only flags
    CXXFLAGS=[
        '-fpermissive',
        '-fno-rtti',
        '-fno-exceptions',
        '-fno-use-cxa-atexit',
    ],
    LINKFLAGS=[
        '-nostdlib',
    ],
    CPPDEFINES={
        'REPO_URL': '\\"$REPO_URL\\"',
        'COMMIT': '\\"$GIT_COMMIT\\"',
        'VER_MAJOR': '\\"${VERSION_ID[0]}\\"',
        'VER_MINOR': '\\"${VERSION_ID[1]}\\"',
        'VER_PATCH': '\\"${VERSION_ID[2]}\\"',
        'VER_NAME': '\\"$VERSION_NAME\\"',
    },
    CPPPATH=[
        '#Kernel',
        '#Thirdparty',
        '#Libraries',
    ],
    LIMINE_INSTALL='#Thirdparty/limine/limine-install-linux-x86_32',

    # ******************************
    # * Compiler Output Formatting *
    # ******************************

    COLOR_COM='\033[0;34m',
    COLOR_OK='\033[0;32m',
    COLOR_INFO='\033[0;93',
    COLOR_NONE='\033[m',

    CXXCOMSTR='  ${COLOR_COM}(CXX)${COLOR_NONE}  $TARGET',
    ASCOMSTR='  ${COLOR_COM}(AS)${COLOR_NONE}   $TARGET',
    CCCOMSTR='  ${COLOR_COM}(CC)${COLOR_NONE}   $TARGET',
    ARCOMSTR='  ${COLOR_COM}(AR)${COLOR_NONE}   $TARGET',
    RANLIBCOMSTR='  ${COLOR_COM}(RL)${COLOR_NONE}   $TARGET',
    LINKCOMSTR='  ${COLOR_COM}(LINK)${COLOR_NONE} $TARGET',
)

env.Append(
    LIBPATH='$INSTALL_DIR',
)

# *******************
# * i686 Toolchains *
# *******************

i686 = env.Clone(
    tools=['nasm'],
    ARCH='i686',
    CXX='i686-elf-g++',
    CC='i686-elf-gcc',
    AR='i686-elf-ar',
    RANLIB='i686-elf-gcc-ranlib',
)

i686.Append(
    ASFLAGS='-felf32',
)

# ************************
# * Kernel Build Targets *
# ************************

targets = [
    i686.Clone(
        tools=['mode_debug'],
    ),
    i686.Clone(
        tools=['mode_release'],
    ),
]

for target_env in targets:
    liballoc = target_env.SConscript(
        'Libraries/liballoc/SConscript',
        variant_dir='$BUILD_DIR/liballoc',
        duplicate=0,
        exports={'env': target_env},
    )
    target_env.Install('$INSTALL_DIR', liballoc)
    kernel = target_env.SConscript(
        'Kernel/SConscript',
        variant_dir='$BUILD_DIR/kernel',
        duplicate=0,
        exports={'env': target_env},
    )
    target_env.Install('$INSTALL_DIR', kernel)
    target_env.EchfsImage(
            '$INSTALL_DIR/xyris',
            [
                '#Kernel/Arch/$ARCH/Bootloader/limine.cfg',
                '#Thirdparty/limine/limine.sys',
                kernel
            ],
            ECHFSFLAGS=['-m', '-p0']
    )
