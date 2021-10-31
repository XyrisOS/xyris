def generate(env):
    env.Replace(
        MODE='release'
    )
    env.Append(
        CPPDEFINES={'RELEASE': None},
        CCFLAGS='-O3',
    )

def exists(env):
    return 1
