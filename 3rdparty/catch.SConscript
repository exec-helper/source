Import('env')

import buildTools

# Download the library
target = env.Command('Catch/include/catch.hpp', 'Catch.commit', 'git submodule init 3rdparty/Catch && git submodule update 3rdparty/Catch')
