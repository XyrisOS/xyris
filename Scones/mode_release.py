def generate(env):
    env.Replace(
        MODE='Release'
    )
    env.Append(
        CPPDEFINES={'RELEASE': None},
        CCFLAGS=[
            '-O3',
        ],
    )

def exists(env):
    return 1
