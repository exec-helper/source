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
        Clang.setupToolChain(env, "")
        env['CXX'] = os.environ['CXX']
        env["ENV"].update(x for x in os.environ.items() if x[0].startswith("CCC_"))
