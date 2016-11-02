class Compiler(object):
    def __init__(self, cCompiler, cxxCompiler, toolchainPath):
        self.cCompiler = cCompiler
        self.cxxCompiler = cxxCompiler
        self.toolchainPath = toolchainPath

    def getCCompiler(self):
        return self.cCompiler

    def getCxxCompiler(self):
        return self.cxxCompiler

    def getToolchainPath(self):
        return self.toolchainPath

class Gcc(Compiler):
    def __init__(self, toolchainPath = None):
        super(self.__class__, self).__init__('gcc', 'g++', toolchainPath)

class Clang(Compiler):
    def __init__(self, toolchainPath = None):
        super(self.__class__, self).__init__('clang', 'clang++', toolchainPath)

class ClangStaticAnalyzer(Compiler):
    def __init__(self, actualCompiler, toolchainPath = None):
        super(self.__class__, self).__init__('scan-build', 'scan-build', toolchainPath)
        self.actualCompiler = actualCompiler
