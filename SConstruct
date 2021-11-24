import os

env = Environment(
    tools=['default', 'echfs'],
    ENV={'PATH': os.environ['PATH']},
    BUILD_DIR='#Build/$ARCH/$MODE',
    INSTALL_DIR='#Distribution/$ARCH/$MODE',
    REPO_URL='https://github.com/XyrisOS/xyris',
    GIT_COMMIT='TODO',
    VERSION_ID=(0, 5, 0),
    VERSION_NAME='Pheonix',
    CCFLAGS=[
        '-nostdlib', '-nodefaultlibs', '-ffreestanding', '-fstack-protector-all',
        '-fno-builtin', '-fno-omit-frame-pointer'
    ],
    CXXFLAGS=[
        '-fpermissive', '-fno-rtti', '-fno-exceptions', '-fno-use-cxa-atexit',
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
        '#kernel',
        '#thirdparty',
        '#libs',
    ],
    CXXCOMSTR='  (CXX)  $TARGET',
    ASCOMSTR='  (AS)   $TARGET',
    CCCOMSTR='  (CC)   $TARGET',
    ARCOMSTR='  (AR)   $TARGET',
    RANLIBCOMSTR='  (RL)   $TARGET',
    LINKCOMSTR='  (LINK) $TARGET',
)

env.Append(
    LIBPATH='$INSTALL_DIR',
)

x86 = env.Clone(
    tools=['nasm'],
    ARCH='i686',
    CXX='i686-elf-g++',
    CC='i686-elf-gcc',
    AR='i686-elf-ar',
    RANLIB="i686-elf-gcc-ranlib",
)

x86.Append(
    ASFLAGS='-felf32',
)

targets = [
    x86.Clone(tools=['mode_debug']),
    x86.Clone(tools=['mode_release']),
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
    target_env.EchfsImage('$INSTALL_DIR/xyris', [kernel])
