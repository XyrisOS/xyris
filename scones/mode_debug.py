def generate(env):
    env.Replace(
        MODE='debug'
    )
    env.Append(
        CPPDEFINES={'DEBUG': None},
        CCFLAGS=['-ggdb3'],
    )

def exists(env):
    return 1
