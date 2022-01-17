from SCons.Script import *
from SCons.Errors import BuildError
from SCons.Action import Action
import os
import platform


disk_size_mb = 2
table_size_mb = 4

def exists(env):
    return True

def generate(env):
    # only run the tool if we exist (seems reasonable)
    assert(exists(env))

    builder = Builder(generator=ext2_image_generator, suffix='.img')
    env.Append(
        BUILDERS={'Ext2Image': builder}
    )

def dd_file_generator(target: File, size: int):
    return Action(
        'dd if=/dev/zero bs=1M count=0 seek={} of={} > /dev/null 2>&1'.format(
            size,
            target.get_path(),
        ),
        '  ${{COLOR_COM}}(DD)${{COLOR_NONE}} {}'.format(
            target.get_path(),
        ),
    )

def partition_table_generator(target: File):
    actions = [
            dd_file_generator(target, table_size_mb),
            Action(
                'parted -s {} mklabel msdos'.format(
                    target.get_path(),
                ),
                '  ${{COLOR_COM}}(PARTED)${{COLOR_NONE}} mklabel {}'.format(
                    target.get_path(),
                )
            ),
            Action(
                'parted -s {} mkpart primary 1 100%'.format(
                    target.get_path(),
                ),
                '  ${{COLOR_COM}}(PARTED)${{COLOR_NONE}} mkpart {}'.format(
                    target.get_path(),
                )
            ),
            Action(
                'parted -s {} set 1 boot on'.format(
                    target.get_path(),
                ),
                '  ${{COLOR_COM}}(PARTED)${{COLOR_NONE}} set boot {}'.format(
                    target.get_path(),
                )
            ),
    ]
    return actions

def ext2_create(target: File):
    return Action(
        'mke2fs {} > /dev/null 2>&1'.format(
            target.get_path(),
        ),
        '  ${{COLOR_COM}}(MKE2FS)${{COLOR_NONE}} {}'.format(
            target.get_path(),
        )
    ),

def ext2_copy(target: File, source: File, path: str):
    return Action(
        'e2cp {} {}:/{}'.format(
            source.get_path(),
            target.get_path(),
            path,
        ),
        '  ${{COLOR_COM}}(E2CP)${{COLOR_NONE}} {}'.format(
            source.get_path(),
        )
    )

def ext2_partition_generator(target_ext2, limine_cfg, limine_sys, kernel):
    actions = [
            Delete(target_ext2),
            dd_file_generator(target_ext2, disk_size_mb),
            ext2_create(target_ext2),
            ext2_copy(target_ext2, limine_cfg, 'limine.cfg'),
            ext2_copy(target_ext2, limine_sys, 'limine.sys'),
            ext2_copy(target_ext2, kernel, 'kernel'),
    ]
    return actions

def dd_file_merger(target, ext2):
    return Action(
        'dd if={} bs=512 seek=2048 of={} 2> /dev/null'.format(
            ext2.get_path(),
            target.get_path(),
        ),
        '  ${{COLOR_COM}}(DD)${{COLOR_NONE}} {}'.format(
            target.get_path(),
        ),
    )

def limine_install(target: File):
    return Action(
        '$LIMINE_INSTALL {} > /dev/null 2>&1'.format(
            target.get_path(),
        ),
        '  ${{COLOR_COM}}(LIMINE)${{COLOR_NONE}} {}'.format(
            target.get_path()
        )
    )

def ext2_image_generator(target, source, env, for_signature):
    assert(len(target) == 1)
    assert(len(source) == 3)
    disk_image = target[0]
    limine_cfg = source[0]
    limine_sys = source[1]
    kernel = source[2]
    target_img = target[0]
    target_ext2 = File(disk_image.get_path() + '.ext2')
    actions = Flatten([
            partition_table_generator(target_img),
            ext2_partition_generator(target_ext2, limine_cfg, limine_sys, kernel),
            dd_file_merger(target_img, target_ext2),
            limine_install(target_img),
    ])
    return actions
