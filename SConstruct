import os

VariantDir('build', 'kernel', duplicate=0)
env = Environment(
    ENV={'PATH': os.environ['PATH']},
    REPO_URL='https://github.com/XyrisOS/xyris',
    GIT_COMMIT='TODO',
    VERSION_ID=(1, 0, 0),
    VERSION_NAME='Pheonix',
    CFLAGS=[
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
    CXX='i686-elf-g++',
    CC='i686-elf-gcc',
    CXXCOMSTR='  (CXX) $SOURCE',
)

env.Append(
        CPPDEFINES={'DEBUG': None},
        CXXFLAGS=['$CFLAGS'],
)

objs = env.Object([
    Glob('build/*.cpp'),
    Glob('build/apps/*.cpp'),
    Glob('build/arch/*.cpp'),
    Glob('build/arch/i386/*.cpp'),
    Glob('build/arch/i386/asm/*.cpp'),
    Glob('build/arch/i386/boot/*.cpp'),
    Glob('build/boot/*.cpp'),
    Glob('build/dev/graphics/*.cpp'),
    Glob('build/dev/rtc/*.cpp'),
    Glob('build/dev/serial/*.cpp'),
    Glob('build/dev/spkr/*.cpp'),
    Glob('build/lib/*.cpp'),
    Glob('build/mem/*.cpp'),
    Glob('build/meta/*.cpp'),
    Glob('build/sys/*.cpp'),
])
