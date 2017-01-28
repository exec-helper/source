Import('env')

import buildTools
import setupTools

yaml_env = env.Clone()

setupTools.disableWarningAsError(yaml_env)

# Download the library
target = yaml_env.Command('yaml-cpp/yaml.h', 'yaml-cpp.commit', 'git submodule init 3rdparty/yaml-cpp && git submodule update 3rdparty/yaml-cpp')

# Build the library
cmake_options = [
    '-DYAML_CPP_BUILD_CONTRIB=OFF',
    '-DYAML_CPP_BUILD_TOOLS=OFF',
    '-DBUILD_SHARED_LIBS=OFF',
    '-DMSVC_SHARED_RT=OFF',
    '-DMSVC_STHREADED_RT=OFF',
    '-Dgtest_force_shared_crt=OFF',
    '-DAPPLE_UNIVERSAL_BIN=OFF'
]

sources = [
    Glob('yaml-cpp/src/*.cc'),
    Glob('yaml-cpp/include/**/*.h')
]

lib = buildTools.buildCmake(    yaml_env, 'libyaml-cpp.a', 
                                sources, 
                                '3rdparty/yaml-cpp',
                                yaml_env['THIRD_PARTY_DIR'] + '/yaml-cpp', 
                                cmake_options,
                                'libyaml-cpp.a'
                        )
                            
installed_lib = yaml_env.Install("{libs_dir}".format(libs_dir=env['THIRD_PARTY_LIBS_DIR']), lib)
yaml_env.Alias('yaml', installed_lib)
