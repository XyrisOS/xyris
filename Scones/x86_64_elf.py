# ******************
# * i686 Toolchain *
# ******************

def generate(env):
    env.SetDefault(
        USEPREFIX=True,
        TCPREFIX="${ARCH + '-elf-' if USEPREFIX else ''}",
    )
    env.Replace(
        ARCH='x86_64',
        CXX='${TCPREFIX}g++',
        CC='${TCPREFIX}gcc',
        AR='${TCPREFIX}ar',
        LD='${TCPREFIX}g++',
        RANLIB='${TCPREFIX}gcc-ranlib',
    )
    env.Append(
        ASFLAGS=[
            '-felf64',
        ],
        CCFLAGS=[
            '-mno-avx',
            '-mno-sse',
        ]
    )

def exists(env):
    return 1
