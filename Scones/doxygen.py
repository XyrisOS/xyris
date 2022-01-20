from SCons.Script import *
from SCons.Errors import BuildError
from SCons.Action import Action

def exists(env):
    return env.Detect("doxygen")

def generate(env):
    assert(exists(env))

    builder = Builder(
        generator=doxygen_generator,
        target_factory=Dir
    )
    env.Append(
        BUILDERS={'Doxygen': builder}
    )

def doxygen_generator(target, source, env, for_signature):
    return Action(
        'doxygen ${SOURCE}',
        '  ${COLOR_COM}(DOXYGEN)${COLOR_NONE} ${SOURCE}'
    )
