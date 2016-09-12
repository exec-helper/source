import os
opts = Variables()

opts.Add(EnumVariable('compiler', 'Set toolchain', 'gcc',
                    allowed_values=('gcc', 'clang', 'cc'),
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

# Tools
tools_list = ['default']

env=Environment(variables=opts, tools = tools_list, ENV = {'PATH' : os.environ['PATH']})
Export('env')

rootDir = Dir('.').abspath
rootBuildDir = Dir('build/{toolchain}/{mode}'.format(toolchain=env['compiler'], mode=env['mode'])).abspath

buildDir = Dir('{rootBuildDir}'.format(rootBuildDir = rootBuildDir)).abspath
sourceDir = Dir('example').abspath
VariantDir('{buildDir}'.format(buildDir=buildDir), sourceDir)

env['ROOT_DIR'] = Dir('.').abspath
env['BUILD_DIR'] = buildDir
env['BIN_DIR'] = '{buildDir}/bin'.format(buildDir = buildDir)

################################## Print stuff ###############################
print("Building in {buildDir}".format(buildDir=buildDir))

################################# Call Sconscript file tree ##########################
SConscript_files = [
	'{buildDir}/SConscript'.format(buildDir=buildDir)
]

SConscript(SConscript_files)
