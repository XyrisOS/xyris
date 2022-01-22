class VariantTool:
    def __init__(self, env):
        dot = env.Dir('.')
        self.abs = str(dot.abspath)
        self.src = str(dot.srcdir)

    def to_src(self, variant):
        rel = '.' + variant.removeprefix(self.abs)
        return os.path.join(self.src, rel)

    def to_variant(self, src):
        rel = '.' + src.removeprefix(self.src)
        return os.path.join(self.abs, rel)


def recursive_glob(env, root: str, extensions: list, ignored_dirs: list=[]):
    """
    Finds all files in a root directory matching the provided file extensions while
    skipping any file within ignored directories.

    Returns a flattened list of File nodes
    """
    sources = []

    rootabs = env.Dir(root).abspath
    vt = VariantTool(env)
    extensions = tuple(extensions)
    for dirpath, dirs, files in os.walk(vt.to_src(rootabs)):
        if any(ignored_dir in dirpath for ignored_dir in ignored_dirs):
            continue
        sources.extend(os.path.normpath(vt.to_variant(os.path.join(dirpath, file))) for file in files if file.endswith(extensions))

    return sources

def generate(env):
    env.AddMethod(recursive_glob, 'RecursiveGlob')

def exists(env):
    return 1
