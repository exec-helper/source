from compilerEnumerations import *

class Gcc:
    def __init__(self, env, toolchain):
        self.setupToolChain(env, toolchain)

    @staticmethod
    def setupToolChain(env, toolchain):
        env['CXX'] = toolchain + 'g++'
        env['AR'] = toolchain + 'ar' 
        env['AS'] = toolchain + 'as' 
        env['LD'] = toolchain + 'ld'
        env['NM'] = toolchain + 'nm' 
        env['STRIP'] = toolchain + 'strip' 

        env['CPPPATH'] = []
        env['CPPFLAGS'] = []
        env['CXXFLAGS'] = []
        env['LDFLAGS'] = []
        env['CFLAGS'] = []
        env['LINKFLAGS'] = []

    @staticmethod
    def getStdLibs(compiler):
        return list(['rt', 'm', 'dl', 'stdc++'])

    @staticmethod
    def enableOptimization(env, optimizationLevel):
        if optimizationLevel is OPTIMIZATION_LEVEL_0:
            env['CPPFLAGS'].append('-O0')
        elif optimizationLevel is OPTIMIZATION_LEVEL_1:
            env['CPPFLAGS'].append('-O1')
        elif optimizationLevel is OPTIMIZATION_LEVEL_2:
            env['CPPFLAGS'].append('-O2')
        elif optimizationLevel is OPTIMIZATION_LEVEL_3:
            env['CPPFLAGS'].append('-O3')
        else:
            print('Warning: optimization level ' + optimizationLevel + ' not supported')

    @staticmethod
    def enableDebuggingSymbols(env):
        env['CPPFLAGS'].append('-g')

    @staticmethod
    def enableWarnings(env, warningLevel):
        if warningLevel >= WARNING_LEVEL_SOME:
            env['CPPFLAGS'].append('-Wall')
        if warningLevel >= WARNING_LEVEL_ALL:
            env['CPPFLAGS'].extend(['-Wextra', '-Wshadow',  '-Wpointer-arith', '-Wcast-qual'])

    @staticmethod
    def disableShadowWarnings(env):
        env['CPPFLAGS'].remove('-Wshadow')

    @staticmethod
    def enableCxxVersion(env, cxxVersion = CXX_VERSION_03):
        if cxxVersion == CXX_VERSION_11:
            env['CPPFLAGS'].append('-std=c++11')

    @staticmethod
    def enableMultiThreading(env, multiThreading):
        if multiThreading == MULTI_THREADING_PTHREAD:
            env['CPPFLAGS'].append(['-pthread'])
        elif multiThreading == MULTI_THREADING_OPENMP:
            env['CPPFLAGS'].append(['-fopenmp'])
            env['LINKFLAGS'].append(['-fopenmp'])

    @staticmethod
    def enableWarningAsError(env):
        env['CPPFLAGS'].append('-Werror')

    @staticmethod
    def disableWarningAsError(env):
        env['CPPFLAGS'].remove('-Werror')

    @staticmethod
    def saveTemps(env):
        env['CPPFLAGS'].extend(['-save-temps', '-fverbose-asm'])

    @staticmethod
    def stopAtAssembler(env):
        env['CPPFLAGS'].extend(['-S', '-fverbose-asm', '-Wa,-adhln'])

    staticmethod
    def enableDiagnostics(env):
        pass    # TODO

    @staticmethod
    def noIgnoredAttribute(env):
        env['CPPFLAGS'].append('-Wno-ignored-attributes')

    @staticmethod
    def enableCodeCoverage(env):
        env['CPPFLAGS'].append('--coverage')
        env['LINKFLAGS'].append('--coverage')

