import os
from compilerEnumerations import *
import gcc

class Clang(gcc.Gcc):
    def __init__(self, env, toolchain):
        self.setupToolChain(env, toolchain)

    @staticmethod
    def setupToolChain(env, toolchain):
        env['CXX'] = toolchain + 'clang++'

        env['CPPPATH'] = []
        env['CPPFLAGS'] = []
        env['CXXFLAGS'] = []
        env['LDFLAGS'] = []
        env['CFLAGS'] = []
        env['LINKFLAGS'] = []

    @staticmethod
    def stopAtAssembler(env):
        env['CPPFLAGS'].extend(['-S', '-fverbose-asm'])

    @staticmethod
    def enableDiagnostics(env):
#        env['CPPFLAGS'].extend(['-Rpass-analysis=loop-vectorize'])
        pass

class ClangStaticAnalyzer(Clang):
    def __init__(self, env):
        self.setupToolChain(env)

    @staticmethod
    def setupToolChain(env):
        env['CXX'] = os.environ['CXX']
        env["ENV"].update(x for x in os.environ.items() if x[0].startswith("CCC_"))

        env['CPPPATH'] = []
        env['CPPFLAGS'] = []
        env['CXXFLAGS'] = []
        env['LDFLAGS'] = []
        env['CFLAGS'] = []
        env['LINKFLAGS'] = []

    @staticmethod
    def disableWarningAsError(env):
        env['CC'] = '/usr/bin/cc'
        env['CXX'] = '/usr/bin/c++'

    def enableWarningAsError(self, env):
        env['CC'] = os.environ['CC']
        env['CXX'] = os.environ['CXX']

        # TODO replace this by a super() call that actually works
        env['CPPFLAGS'].append('-Werror')
