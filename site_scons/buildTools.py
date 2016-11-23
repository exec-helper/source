import copy     # For some reason importing this does not work
import os

from SCons.Script import *
import scons_utils
import setupTools

def buildCmake(env, output, sources, cmakeRootDir, buildDir, cmakeOptions, outputAfterCmakeBuild):
    return env.Command(output, sources, 
    [
        'echo "Building ' + output + '"',
        'mkdir -p ' + buildDir,
        'cmake -B' + buildDir + ' ' + '-H' + cmakeRootDir +' ' + 
            '-DCMAKE_BUILD_TYPE=Release ' +
            scons_utils.listToString(cmakeOptions, ' ') +
            '-DCMAKE_C_COMPILER=' + env['CC'] + ' ' +
            '-DCMAKE_CXX_COMPILER=' + env['CXX'],
        'make -C' + buildDir + ' -j8',
        'cp ' + buildDir + '/' + outputAfterCmakeBuild + ' $TARGET'
    ])

def generateIncludeUsageRequirementName(libName):
    return 'LIB_' + libName.upper() + '_INCLUDES'

def generateLibUsageRequirementName(libName):
    return 'LIB_' + libName.upper() + '_LIB'

def createIncludeUsageRequirement(env, libName, includes):
    lib_include_requirements_name = generateIncludeUsageRequirementName(libName)

    if lib_include_requirements_name in env: 
        env[lib_include_requirements_name].extend(includes)
    else:
        env[lib_include_requirements_name] = includes

def createLibUsageRequirement(env, libName, libs):
    lib_lib_requirements_name = generateLibUsageRequirementName(libName)

    if lib_lib_requirements_name in env: 
        env[lib_lib_requirements_name].extend(libs)
    else:
        env[lib_lib_requirements_name] = libs

def addIncludeUsageRequirement(env, lib, toAdd):
    lib_include_requirements_name = generateIncludeUsageRequirementName(lib)
    if lib_include_requirements_name in env: 
        toAdd.extend(env[lib_include_requirements_name])

def addLibUsageRequirement(env, lib):
    toAdd = []
    lib_lib_requirements_name = generateLibUsageRequirementName(lib)
    if lib_lib_requirements_name in env: 
        toAdd.extend(env[lib_lib_requirements_name])
    return toAdd

def createUnittest(env, name, sources, includes, libs, libs_path):
    env_unittest = env.Clone()

    env_unittest['CPPPATH'].append(env['ROOT_DIR'] + '/3rdparty/Catch/include')

    libs_unittest = copy.deepcopy(libs)
    libs_unittest.extend(env['LIB_UNITTEST'])

    target = createProgram(env_unittest, name, sources, includes, libs_unittest, libs_path, env['UNITTEST_BIN_DIR'])
    env.Alias("unittests", target)
    return target

def createBenchmark(env, name, sources, includes, libs, libs_path, TEMPS = []):
    env_benchmark = env.Clone()
    libs_benchmark = copy.deepcopy(libs)

    env_benchmark['CPPPATH'].extend(includes)

    addIncludeUsageRequirement(env, 'benchmark', env['CPPPATH'])
    addIncludeUsageRequirement(env_benchmark, 'benchmark', env_benchmark['CPPPATH'])
    for lib in libs:
        addIncludeUsageRequirement(env_benchmark, lib, env_benchmark['CPPPATH'])
        libs_unittest.extend(addLibUsageRequirement(env_unittest, lib))

    libs_benchmark.extend(env['LIB_BENCHMARK'])
    libs_benchmark.extend(env['STD_LIBS'])


    target = env_benchmark.Program(name, sources, LIBS=libs_benchmark, LIBPATH=libs_path)
    installed_bin = env_benchmark.Install("{bin_dir}".format(bin_dir=env['BENCHMARK_BIN_DIR']), target)
    env_benchmark.Alias(name, installed_bin)
    env_benchmark.Alias("benchmarks", installed_bin)

    targetList = [installed_bin]

    # Create temps
    for temp in TEMPS:
        if temp == '.s' or temp == '.S':
            asm_files = createAssembler(env, name, sources, includes, libs, libs_path)
            targetList.append(asm_files)

    return targetList

def createAssembler(env, name, sources, includes, libs, libs_path):
    env_benchmark = env.Clone()

    env_benchmark['CPPPATH'].extend(includes)
    setupTools.stopAtAssembler(env_benchmark)

    for lib in libs:
        addIncludeUsageRequirement(env_benchmark, lib, env_benchmark['CPPPATH'])

    # Build target sources
    targets = []
    for source in sources:
        targetFile = os.path.splitext(source.rstr())[0]+'.s'
        target = env_benchmark.StaticObject(targetFile, source )
        targets.append(target)
        env.Alias(name, target)
        env.Alias("benchmarks", target)

    return targets

def createLib(env, name, sources, includes, global_env, includeUsageRequirements, libUsageRequirements):
    createIncludeUsageRequirement(global_env, name, includeUsageRequirements)
    createLibUsageRequirement(global_env, name, libUsageRequirements)

    env_lib = env.Clone()
    env_lib['CPPPATH'].extend(includes)

    lib = env_lib.Library(name, sources)
    installed_lib = env.Install("{libs_dir}".format(libs_dir=env['LIBS_DIR']), lib)
    env.Alias(name, installed_lib)
    env.Alias('buildLibs', installed_lib)

def createProgram(env, name, sources, includes, libs, libs_path, install_dir):
    env_program = env.Clone()
    libs_program = copy.deepcopy(libs)

    env_program['CPPPATH'].extend(includes)

    for lib in libs:
        addIncludeUsageRequirement(env_program, lib, env_program['CPPPATH'])
        libs_program.extend(addLibUsageRequirement(env_program, lib))

    libs_program.extend(env['STD_LIBS'])

    target = env_program.Program(name, sources, LIBS=libs_program, LIBPATH=libs_path)
    installed_bin = env.Install("{bin_dir}".format(bin_dir=install_dir), target)
    env.Alias(name, installed_bin)
    return installed_bin
