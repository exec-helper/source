import copy     # For some reason importing this does not work
import os

from SCons.Script import *
import scons_utils
import setupTools

class LibUsageRequirements:
    def __init__(self, name, includeUsageRequirements, libUsageRequirements, libPathUsageRequirements, dependencies):
        self.name = name
        self.includeUsageRequirements = includeUsageRequirements
        self.libUsageRequirements = libUsageRequirements
        self.libPathUsageRequirements = libPathUsageRequirements
        self.dependencies = dependencies

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

def generateLibUsageRequirementName(libName):
    return 'LIB_USAGE_REQUIREMENTS_' + libName.upper()

def createUsageRequirements(env, name, includeUsageRequirements, libUsageRequirements, libPathUsageRequirements, dependencies):
    libUsageRequirementName = generateLibUsageRequirementName(name)
    env[libUsageRequirementName] = LibUsageRequirements(name, includeUsageRequirements, libUsageRequirements, libPathUsageRequirements, dependencies)

def addIncludeUsageRequirement(env, dependency):
    toAdd = []
    libUsageRequirementName = generateLibUsageRequirementName(dependency)
    if libUsageRequirementName in env: 
        if env[libUsageRequirementName].includeUsageRequirements is not None:
            toAdd.extend(env[libUsageRequirementName].includeUsageRequirements)

        if env[libUsageRequirementName].dependencies is not None:
            for dependentDependency in env[libUsageRequirementName].dependencies:
                toAdd.extend(addIncludeUsageRequirement(env, dependentDependency))
    return toAdd

def addLibUsageRequirement(env, dependency):
    toAdd = []
    libUsageRequirementName = generateLibUsageRequirementName(dependency)
    if libUsageRequirementName in env: 
        if env[libUsageRequirementName].libUsageRequirements is not None:
            toAdd.extend(env[libUsageRequirementName].libUsageRequirements)

        if env[libUsageRequirementName].dependencies is not None:
            for extraLib in env[libUsageRequirementName].dependencies:
                toAdd.extend(addLibUsageRequirement(env, extraLib))
    return toAdd

def addLibPathUsageRequirement(env, dependency):
    toAdd = []
    libUsageRequirementName = generateLibUsageRequirementName(dependency)
    if libUsageRequirementName in env: 
        if env[libUsageRequirementName].libPathUsageRequirements is not None:
            toAdd.extend(env[libUsageRequirementName].libPathUsageRequirements)

        if env[libUsageRequirementName].dependencies is not None:
            for extraLib in env[libUsageRequirementName].dependencies:
                toAdd.extend(addLibPathUsageRequirement(env, extraLib))
    return toAdd

def createUnittest(env, name, sources, includes, libs, libs_path, dependencies):
    env_unittest = env.Clone()

    env_unittest['CPPPATH'].append(env['ROOT_DIR'] + '/3rdparty/Catch/include')

    if libs is not None:
        libs_unittest = copy.deepcopy(libs)
    else:
        libs_unittest = []
    libs_unittest.extend(env['LIB_UNITTEST'])

    target = createProgram(env_unittest, name, sources, includes, libs_unittest, libs_path, env['UNITTEST_BIN_DIR'], dependencies)
    env.Alias("unittests", target)
    env.Alias("test", target)
    return target

def createLib(env, name, sources, includes, global_env, includeUsageRequirements, libUsageRequirements, libPathUsageRequirements, dependencies):
    createUsageRequirements(global_env, name, includeUsageRequirements, libUsageRequirements, libPathUsageRequirements, dependencies)

    env_lib = env.Clone()
    env_lib['CPPPATH'].extend(includes)

    if dependencies is not None:
        for dependency in dependencies:
            env_lib['CPPPATH'].extend(addIncludeUsageRequirement(env_lib, dependency))

    lib = env_lib.Library(name, sources)
    installed_lib = env.Install("{libs_dir}".format(libs_dir=env['LIBS_DIR']), lib)
    env.Alias(name, installed_lib)
    env.Alias('buildLibs', installed_lib)

def createProgram(env, name, sources, includes, libs, libs_path, install_dir, dependencies):
    env_program = env.Clone()

    if libs is not None:
        libs_program = copy.deepcopy(libs)
    else:
        libs_program = []

    if libs_path is None:
        libs_path = []

    if includes is not None:
        env_program['CPPPATH'].extend(includes)

    for dependency in dependencies:
        env_program['CPPPATH'].extend(addIncludeUsageRequirement(env_program, dependency))
        libs_program.extend(addLibUsageRequirement(env_program, dependency))
        libs_path.extend(addLibPathUsageRequirement(env_program, dependency))

    libs_program.extend(env['STD_LIBS'])
    libs_path.extend([env['LIBS_DIR'], env['THIRD_PARTY_LIBS_DIR']])

    target = env_program.Program(name, sources, LIBS=libs_program, LIBPATH=libs_path)
    installed_bin = env.Install("{bin_dir}".format(bin_dir=install_dir), target)
    env.Alias(name, installed_bin)
    return installed_bin
