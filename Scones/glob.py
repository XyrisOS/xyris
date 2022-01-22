class VariantTool:
    """
    VariantTool is a class designed to help translate paths for variant builds
    to their appropriate location in the source tree. The current directory for
    any given build is the Variant Directory, which normally contains all source
    SCons deemed appropriate to build the program variant.

    This tool translates source paths that should live within the variant directory to
    their corresponding paths from the project root (as opposed to from the variant
    build directory)
    """

    def __init__(self, env):
        dot = env.Dir('.')
        self.abs = str(dot.abspath)
        self.src = str(dot.srcdir)

    def to_src(self, variant):
        """
        Translate a variant build path into the corresponding path in the root source tree
        """
        rel = '.' + variant.removeprefix(self.abs)
        return os.path.join(self.src, rel)

    def to_variant(self, src):
        """
        Translate a source tree path into the corresponding variant build path
        """
        rel = '.' + src.removeprefix(self.src)
        return os.path.join(self.abs, rel)


def recursive_glob(env, root: str, extensions: list, ignored_dirs: list=[]):
    """
    Finds all files in a root directory matching the provided file extensions while
    skipping any file within ignored directories.

    Returns a flattened list of paths
    """
    sources = []

    rootabs = env.Dir(root).abspath
    vt = VariantTool(env)
    extensions = tuple(extensions)
    for dirpath, dirs, files in os.walk(vt.to_src(rootabs)):
        if any(ignored_dir in dirpath for ignored_dir in ignored_dirs):
            continue
        """
        For every file in the list of files found within the directory, if the extension exists within the list of desired extensions,

        Create the full path by combining the directory path and filename (with extension). From there, translate this path to the
        corresponding location in the variant build directory (so that SCons is made aware that it is required for the given variant).

        Finally, normalize the path and add it to the list of discovered source files.
        """
        sources.extend(os.path.normpath(vt.to_variant(os.path.join(dirpath, file))) for file in files if file.endswith(extensions))

    return sources

def generate(env):
    env.AddMethod(recursive_glob, 'RecursiveGlob')

def exists(env):
    return 1
