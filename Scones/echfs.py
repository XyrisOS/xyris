from SCons.Script import *
from SCons.Errors import BuildError
from SCons.Action import Action
import os
import platform


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
    bs = '1M'
    # Take macOS's dd differences into account
    if platform.system() == 'Darwin':
        bs = '1m'
    actions = [
            Action(
                'dd if=/dev/zero bs={} count=0 seek=2 of={} 2> /dev/null'.format(bs, target[0]),
                '  (DD) {}'.format(target[0])
            ),
            Action('parted -s $TARGET mklabel msdos', '  (PART) mklabel $TARGET'),
            Action('parted -s $TARGET mkpart primary 1 100%', '  (PART) mkpart $TARGET'),
            Action('parted -s $TARGET set 1 boot on', '  (PART) set boot $TARGET'),
            echfs_format(),
    ]
    for src in source:
        #print('echfs_image_gen: src: {}'.format(src))
        dest = os.path.basename(str(src))
        actions.append(echfs_import(dest, src))
    actions.append(Action('$LIMINE_INSTALL $TARGET', '  (LIMINE) $TARGET'))
    return actions
