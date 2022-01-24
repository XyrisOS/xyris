# ******************
# * i686 Toolchain *
# ******************

def generate(env):
    env.Replace(
        ARCH='i686',
        CXX='i686-elf-g++',
        CC='i686-elf-gcc',
        AR='i686-elf-ar',
        LD='i686-elf-g++',
        RANLIB='i686-elf-gcc-ranlib',
    )
    env.Append(
        ASFLAGS=[
            '-felf32',
        ],
        CCFLAGS=[
            '-mno-avx',
            '-mno-sse',
        ]
    )

def exists(env):
    return 1
