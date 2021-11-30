def generate(env):
    env.Replace(
        MODE='Release'
    )
    env.Append(
        CPPDEFINES={'RELEASE': None},
        CCFLAGS=[
            '-O3',
            '-mno-avx',
        ],
    )

def exists(env):
    return 1
