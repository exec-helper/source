import os
opts = Variables()

import setupTools

# Note: CC will take the CC variable in the environment, cc will take the cc program in the path
opts.Add(EnumVariable('compiler', 'Set toolchain', 'gcc',
                    allowed_values=('gcc', 'clang', 'cc', 'envcc'),
                    map={},
                    ignorecase=2))

opts.Add(EnumVariable('toolchainPrefix', 'Set toolchain prefix', '',
                    allowed_values=('', 'arm-bcm2708hardfp-linux-gnueabi'),
                    map={},
                    ignorecase=2))

opts.Add(PathVariable('toolchainPath', 'Set toolchain path', '/usr/bin'))


opts.Add(EnumVariable('mode', 'Set build mode', 'debug',
                    allowed_values=('debug', 'release'),
                    map={},
                    ignorecase=2))

opts.Add(EnumVariable('profile', 'Set profile flag', 'no',
					allowed_values=('yes', 'no', 'gprof', 'perf', 'callgrind'),
					map = {},
					ignorecase=2))

opts.Add(EnumVariable('multithreading', 'Set multithreading', 'none',
					allowed_values=('none', 'openmp'),
					map = {},
					ignorecase=2))

opts.Add(EnumVariable('verbose', 'Set verbosity of scons output', 'no',
					allowed_values=('yes', 'no'),
					map = {},
					ignorecase=2))



# Tools
tools_list = ['default']

env=Environment(variables=opts, tools = tools_list, ENV = {'PATH' : os.environ['PATH']})
Export('env')

if env['verbose'] == 'no':
    env['CCCOMSTR'] = "Compiling $TARGET"
    env['CXXCOMSTR'] = "Compiling $TARGET"
    env['LINKCOMSTR'] = "Linking $TARGET"

setupTools.setupToolchain(env, env['compiler'], env['toolchainPrefix'], env['toolchainPath'])

rootDir = Dir('.').abspath
rootBuildDir = Dir('build/{toolchain}/{mode}'.format(toolchain=env['COMPILER_NAME'], mode=env['mode'])).abspath

# Make a separate one when profiling is set, since this often requires one to toggle between builds
if env['profile'] != 'no':
	rootBuildDir += '/profile'

buildDir = Dir('{rootBuildDir}'.format(rootBuildDir = rootBuildDir)).abspath
sourceDir = Dir('src').abspath
thirdpartyDir = Dir('3rdparty').abspath
thirdpartyBuildDir = '{rootBuildDir}/3rdparty'.format(rootBuildDir = rootBuildDir)
VariantDir('{buildDir}'.format(buildDir=buildDir), sourceDir)
VariantDir('{thirdpartyDir}'.format(thirdpartyDir=thirdpartyBuildDir), thirdpartyDir)

env['ROOT_DIR'] = Dir('.').abspath
env['BUILD_DIR'] = buildDir
env['THIRD_PARTY_DIR'] = '{thirdpartyBuildDir}'.format(thirdpartyBuildDir=thirdpartyBuildDir)
env['THIRD_PARTY_INCLUDE_DIR'] = '{thirdpartyBuildDir}/include'.format(thirdpartyBuildDir=thirdpartyBuildDir)
env['THIRD_PARTY_LIBS_DIR'] = '{thirdpartyBuildDir}/libs'.format(thirdpartyBuildDir=thirdpartyBuildDir)
env['LIBS_DIR'] = '{buildDir}/libs'.format(buildDir = buildDir)
env['BIN_DIR'] = '{buildDir}/bin'.format(buildDir = buildDir)
env['UNITTEST_BIN_DIR'] = '{bindir}/test/unittest'.format(bindir = env['BIN_DIR'])

env['CPPPATH'].append('{buildDir}'.format(buildDir=buildDir))
env['CPPPATH'].append('{thirdpartyBuildDir}'.format(thirdpartyBuildDir=env['THIRD_PARTY_INCLUDE_DIR']))

# Add thirdparties to include path
env['CPPPATH'].append('{thirdPartyDir}/{catchIncludeDir}'.format(thirdPartyDir = env['THIRD_PARTY_DIR'], catchIncludeDir = "Catch/include"))

# Fix for 3rd party modules that actually want to be installed in the system dirs
env['CXXFLAGS'].append(['-isystem{thirdpartyBuildDir}'.format(thirdpartyBuildDir=env['THIRD_PARTY_INCLUDE_DIR'])])

env['STD_LIBS'] = setupTools.getStdLibs(env, env['COMPILER_FAMILY'])

setupTools.enableWarnings(env)
setupTools.enableWarningAsError(env)
setupTools.enableCxxVersion(env, setupTools.CXX_VERSION_11)

if env['mode'] == 'release':
    setupTools.enableOptimization(env, setupTools.OPTIMIZATION_LEVEL_3)
else:
    setupTools.enableOptimization(env)
    setupTools.enableDebuggingSymbols(env)
    setupTools.enableCodeCoverage(env)

##################################### Profiling related stuff ##############################
# Default when profiling is enabled: perf
if env['profile'] == 'yes':
	env['profile'] = 'perf'

if env['profile'] != 'no':
	# Add debugging symbols to enable more useful information when profiling
	env['CPPFLAGS'].append('-g')
	# Disable inlining
	env['CPPFLAGS'].append('-fno-inline')

if env['profile'] == 'gprof':
	env['CFLAGS'].append('-pg')
	env['CPPFLAGS'].append('-pg')
	env['LINKFLAGS'].append('-pg')

################################## Multi-threading related stuff ############################@
if env['multithreading'] == 'openmp':
    setupTools.enableMultiThreading(MULTI_THREADING_OPENMP)

################################## Target specific stuff ##################################   
#if(toolchainPrefix == 'arm-bcm2708hardfp-linux-gnueabi-'):
#    env['CPPFLAGS'].append(['-mfpu=vfp', '-mfloat-abi=hard', '-march=armv6zk', '-mtune=arm1176jzf-s'])
    # Suppress mangling va thing
#    env['CPPFLAGS'].append('-Wno-psabi')


################################## Print stuff ###############################
print("Building in {buildDir}".format(buildDir=buildDir))

setupTools.checkSubmodules(env['BUILD_DIR'])

################################# Call Sconscript file tree ##########################
SConscript_files = [
	'{thirdpartyBuildDir}/SConscript'.format(thirdpartyBuildDir = thirdpartyBuildDir),
	'{buildDir}/SConscript'.format(buildDir=buildDir),
]

SConscript(SConscript_files)
