from SCons.Script import *
from SCons.Errors import BuildError
from SCons.Action import Action
import os


def exists(env):
    return True

def generate(env):
    # only run the tool if we exist (seems reasonable)
    assert(exists(env))

    builder = Builder(generator=echfs_image_generator, suffix='.img')
    env.Append(BUILDERS={'EchfsImage': builder})
    env.SetDefault(
            ECHFS='echfs-utils',
            ECHFSFLAGS=[],
            ECHFSCOMSTR='  (ECHFS)',
            ECHFS_BLOCK_SIZE='32768',
    )

def echfs_format():
    return Action(
           echfs_op('quick-format', '$ECHFS_BLOCK_SIZE'),
           '$ECHFSCOMSTR [format] $TARGET',
    )

def echfs_import(target, source):
    return Action(
           echfs_op('import', source, target),
           '$ECHFSCOMSTR [add] $TARGET({})'.format(target),
    )

def echfs_op(action, *args):
    return '$ECHFS $ECHFSFLAGS $TARGET {} {} > /dev/null'.format(action, ' '.join([str(a) for a in args]))

def echfs_image_generator(target, source, env, for_signature):
    assert(len(target) == 1)
    actions = [
            Action('dd if=/dev/zero bs=1M count=0 seek=2 of={} 2> /dev/null'.format(target[0]), '  (DD) {}'.format(target[0])),
            echfs_format(),
    ]
    for src in source:
        #print('echfs_image_gen: src: {}'.format(src))
        dest = os.path.basename(str(src))
        actions.append(echfs_import(dest, src))
    return actions
