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
import subprocess

def get_git_commit():
    """Returns the current git version as a string."""
    return subprocess.check_output(
        ['git', 'describe', '--abbrev=8', '--dirty', '--always', '--tags']
    ).decode().rstrip()

# Clear any default targets set by SCons
Default(None)

env = Environment(
    tools=[
        'default',
        'doxygen',
        'colors',
        'glob',
        'ext2'
    ],
    toolpath=[
        'Scones',
    ],
    ENV={
        'PATH': os.environ['PATH'],
    },
    BUILD_DIR='#Build/$ARCH/$MODE',
    INSTALL_DIR='#Distribution/$ARCH/$MODE',
    LIBPATH='$INSTALL_DIR',
    REPO_URL='https://git.io/JWjEx',
    GIT_COMMIT=get_git_commit(),
    VERSION_ID=(0, 5, 0),
    VERSION_NAME='Tengu',

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
        '-fstack-protector',
        '-fno-builtin',
        '-fno-omit-frame-pointer',
        '${CCWARNINGS}'
    ],
    # C++ only flags
    CXXFLAGS=[
        '-std=c++20',
        '-fpermissive',
        '-fno-rtti',
        '-fno-exceptions',
        '-fno-use-cxa-atexit',
    ],
    LINKFLAGS=[
        '-nostdlib',
        '-lgcc',
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
    LIMINE_INSTALL=File('#Thirdparty/limine/limine-install-linux-x86_32'),
)

# ************************
# * Kernel Build Targets *
# ************************

kernel_environments = [
    # i686 ELF (debug)
    env.Clone(
        tools=[
            'nasm',
            'i686_elf',
            'mode_debug',
        ],
    ),
    # i686 ELF (release)
    env.Clone(
        tools=[
            'nasm',
            'i686_elf',
            'mode_release',
        ],
    ),
]

# Allow docs to be built without the kernel or a compiler
# This is necessary for allowing CI to build and publish docs
if 'docs' not in COMMAND_LINE_TARGETS:
    # Build a list of all build targets associated with the kernel
    # Includes all dependencies, kernels, bootable images, etc.
    kernel_targets_all = []
    kernel_targets_debug = []
    kernel_targets_release = []
    for target_env in kernel_environments:
        liballoc = target_env.SConscript(
            'Libraries/liballoc/SConscript',
            variant_dir='$BUILD_DIR/liballoc',
            duplicate=0,
            exports={
                'env': target_env
            },
        )
        Default(liballoc)
        target_env.Install('$INSTALL_DIR', liballoc)
        kernel = target_env.SConscript(
            'Kernel/SConscript',
            variant_dir='$BUILD_DIR/kernel',
            duplicate=0,
            exports={
                'env': target_env
            },
        )
        Default(kernel)
        target_env.Install('$INSTALL_DIR', kernel)
        image = target_env.Ext2Image(
                '$INSTALL_DIR/xyris.img',
                [
                    '#Kernel/Arch/$ARCH/Bootloader/limine.cfg',
                    '#Thirdparty/limine/limine.sys',
                    kernel
                ]
        )
        Default(image)

        kernel_targets_all.extend([liballoc, kernel, image])

        # Add targets to kernel_targets_[MODE] list
        target_list_name = 'kernel_targets_' + target_env['MODE'].lower()
        targets_list = globals()[target_list_name]
        targets_list.extend([liballoc, kernel, image])

    # Mode specific kernel targets
    env.Alias('kernel-debug', kernel_targets_debug)
    env.Alias('kernel-release', kernel_targets_release)

# ************************
# * Kernel Phony Targets *
# ************************

# Allow user to specify the mode. Don't want to overwrite the actual 'MODE' construction
# variable though, so we alias the environment variable 'MODE' to 'RUN_MODE'
env['RUN_MODE'] = ARGUMENTS.get('MODE', 'Release')

env.Pseudo(
    env.Command(
        'run-i686',
        None,
        '''
        qemu-system-x86_64 \
        -drive file=Distribution/i686/${RUN_MODE}/xyris.img,index=0,media=disk,format=raw \
        -m 4G \
        -rtc clock=host \
        -vga std \
        -serial stdio \
        -monitor telnet:127.0.0.1:1234,server,nowait \
        &
        '''
    )
)

env.Pseudo(
    env.Command(
        'debug-i686',
        None,
        '''
        qemu-system-x86_64 \
        -S -s \
        -drive file=Distribution/i686/${RUN_MODE}/xyris.img,index=0,media=disk,format=raw \
        -m 4G \
        -rtc clock=host \
        -vga std \
        -serial stdio \
        &
        '''
    )
)

# ************************
# * Kernel Documentation *
# ************************

kernel_docs = env.Doxygen(
    '#Documentation/Build',
    '#Documentation/Doxyfile',
)
env.Alias('docs', kernel_docs)
env.Clean('docs', '#Documentation/Build')

# ***************************
# * Unit Test Build Targets *
# ***************************

env = Environment(
    tools=[
        'default',
        'colors',
        'glob'
    ],
    toolpath=[
        'Scones',
    ],
    BUILD_DIR='#Build/Tests',
    INSTALL_DIR='#Distribution/Tests',
    LIBPATH='$INSTALL_DIR',
    CXXFLAGS=[
        '-fprofile-arcs',
        '-ftest-coverage',
    ],
    CPPPATH=[
        '#Tests',
        '#Kernel',
        '#Thirdparty',
    ],
    LINKFLAGS=[
        '--coverage',
        '-lstdc++',
        '-lgcov',
        '-lgcc',
        '-lm',
    ],
)

catch2_header_url = 'https://github.com/catchorg/Catch2/releases/download/v2.13.8/catch.hpp'
catch2_header = env.Command(
    '#Thirdparty/catch2/catch.hpp',
    None,
    'wget -qP ${{TARGET.dir}} {}'.format(catch2_header_url)
)

tests = env.SConscript(
    'Tests/SConscript',
    variant_dir='$BUILD_DIR/tests',
    duplicate=0,
    exports={
        'env': env
    }
)
tests_intall = env.Install('$INSTALL_DIR', tests)
env.Alias('tests', [catch2_header, tests, tests_intall])
