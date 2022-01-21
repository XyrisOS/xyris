# ******************************
# * Compiler Output Formatting *
# ******************************

def generate(env):
    env.SetDefault(
        COLOR_COM='\033[0;34m',
        COLOR_OK='\033[0;32m',
        COLOR_INFO='\033[0;93',
        COLOR_NONE='\033[m',
    )
    env.Replace(
        CXXCOMSTR='  ${COLOR_COM}(CXX)${COLOR_NONE}  $TARGET',
        ASCOMSTR='  ${COLOR_COM}(AS)${COLOR_NONE}   $TARGET',
        CCCOMSTR='  ${COLOR_COM}(CC)${COLOR_NONE}   $TARGET',
        ARCOMSTR='  ${COLOR_COM}(AR)${COLOR_NONE}   $TARGET',
        RANLIBCOMSTR='  ${COLOR_COM}(RL)${COLOR_NONE}   $TARGET',
        LINKCOMSTR='  ${COLOR_COM}(LINK)${COLOR_NONE} $TARGET',
    )

def exists(env):
    return 1
