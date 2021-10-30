import os

env = Environment(
    ENV={'PATH': os.environ['PATH']},
    BUILD_DIR='#build/$ARCH/$MODE',
    INSTALL_DIR='#dist/$ARCH/$MODE',
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
    ],
    CXXCOMSTR='  (CXX) $SOURCE',
    ASCOMSTR='  (AS) $SOURCE',
)

env.Append(
    CPPDEFINES={'DEBUG': None},
)

x86 = env.Clone(
    tools=['nasm'],
    ARCH='i686',
    CXX='i686-elf-g++',
    CC='i686-elf-gcc',
)

x86.Append(
    ASFLAGS='-f elf32',
)

targets = [
    x86.Clone(MODE='debug'),
    #x86.Clone(MODE='release'),
]

for target_env in targets:
    target_env.SConscript(
        'kernel/SConscript',
        variant_dir='$BUILD_DIR/kernel',
        duplicate=0,
        exports={'env': target_env},
    )
