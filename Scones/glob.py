def glob(env, root: str, extensions: list):
    """
    Finds all files in a root directory matching the provided file extensions

    Returns a flattened list of File nodes
    """
    sources = []
    for (dirpath, dirs, files) in os.walk(root):
        for extension in extensions:
            sources.append(env.Glob(dirpath + '/*' + extension))
    return env.Flatten(sources)

def glob_with_ignored_dirs(env, root: str, extensions: list, ignored_dirs: list):
    """
    Finds all files in a root directory matching the provided file extensions while
    skipping any file within ignored directories.

    Returns a flattened list of File nodes
    """
    sources = []
    for (dirpath, dirs, files) in os.walk(root):
        for ignored_dir in ignored_dirs:
            if not dirpath.startswith(ignored_dir):
                for extension in extensions:
                    sources.append(env.Glob(dirpath + '/*' + extension))
    return env.Flatten(sources)


def generate(env):
    env.AddMethod(glob, 'glob')
    env.AddMethod(glob_with_ignored_dirs, 'glob_with_ignored_dirs')

def exists(env):
    return 1
