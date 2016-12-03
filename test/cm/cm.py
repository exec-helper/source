import os

from exec_utils.util.util import *

from .target import *

SHELL_RETURN_CODE = 0

PYTHON2 = ['python2']
PYTHON3 = ['python3']
PYTHON_COVERAGE = ['coverage', 'run', '-p']

GCC = 'gcc'
CLANG = 'clang'

DISTCLEAN_COMMAND = 'distclean'
CLEAN_COMMAND = 'clean'
BUILD_COMMAND = 'build'
RUN_COMMAND = 'run'

class UnittestOptions:
    compilers = []
    pythonVersion = None

    @staticmethod
    def addCompiler(compiler):
        UnittestOptions.compilers.append(compiler)

    @staticmethod
    def getCompilers():
        return UnittestOptions.compilers

    @staticmethod
    def setPythonVersion(newPythonVersion):
        if newPythonVersion == PYTHON2[0]:
            UnittestOptions.pythonVersion = PYTHON2
        elif newPythonVersion == PYTHON3[0]:
            UnittestOptions.pythonVersion = PYTHON3
        elif newPythonVersion == PYTHON_COVERAGE[0]:
            UnittestOptions.pythonVersion = PYTHON_COVERAGE
        else:
            raise ValueError("Invalid python version: '" + newPythonVersion + "'")

    @staticmethod
    def getPythonVersion():
        return UnittestOptions.pythonVersion


def getTestRootDir():
    return '.'

def writeFile(filename, content, writeMode='w'):
    directory = os.path.dirname(filename)
    if not os.path.exists(directory):
        os.makedirs(directory)

    with open(filename, writeMode) as f:
        if content:
            try:
                f.write(content)
            except:
                f.write(content.decode('ascii'))
        else:
            f.write("")

def execute(cmd, outputFile = None, errorFile = None):
    retCode,output,error = getShellOutputAndReturnCode(cmd, getTestRootDir(), False)
    if outputFile:
        writeFile(outputFile, output)

    if errorFile:
        writeFile(errorFile, error)
    return retCode == SHELL_RETURN_CODE

def executeTarget(commandList, target, pythonVersion = PYTHON3, outputFile = None, errorFile = None, execCommand = 'exec'):
    cmd = []
    cmd.extend(pythonVersion)
    cmd.append(execCommand)
    cmd.extend(commandList)
    cmd.extend(target.getProfileMapOptions())
    cmd.extend(target.getCompilerOptions())
    cmd.extend(target.getTargetOptions())
    cmd.extend(target.getRunTargetOptions())
    cmd.extend(target.getModeOptions())
    cmd.extend(target.getBuilderOptions())
    cmd.extend(target.getRootBuildDirOptions())
    cmd.extend(target.getArchitectureOptions())
    cmd.extend(target.getDistributionOptions())
#    print(' '.join(cmd))
    return execute(cmd, outputFile, errorFile)

def checkThatFileExists(path):
    fileExists = os.path.isfile(path)
    return fileExists

def checkThatDirExists(path):
    return os.path.isdir(path)

COMPILER_DEBUG_MODE = 'debug'
COMPILER_RELEASE_MODE = 'release'

class Compiler(object):
    def __init__(self, mode, name, toolchainPath):
        self.mode = mode
        self.name = name
        self.toolchainPath = toolchainPath

    def getName(self):
        return self.name

    def getMode(self):
        return self.mode

    def getToolchainPath(self):
        return self.toolchainPath

    def __repr__(self):
        return "Compiler: name = " + self.name + ", mode = " + self.mode

    @staticmethod
    def getAllCompilers():
        return  [
                    Gcc(COMPILER_RELEASE_MODE, '/all/compilers/'), 
                    Gcc(COMPILER_DEBUG_MODE, '/all/compilers/'),
                    Clang(COMPILER_RELEASE_MODE, '/all/compilers/'),
                    Clang(COMPILER_DEBUG_MODE, '/all/compilers/')
                ]

class Gcc(Compiler):
    def __init__(self, mode = 'release', toolchainPath = None):
        super(Gcc, self).__init__(mode, 'gcc', toolchainPath)

class Clang(Compiler):
    def __init__(self, mode = 'release', toolchainPath = None):
        super(Clang, self).__init__(mode, 'clang', toolchainPath)
