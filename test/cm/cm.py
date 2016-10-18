import os

from exec_utils.util.util import *

from cm.target import *

SHELL_RETURN_CODE = 0

PYTHON2 = 'python2'
PYTHON3 = 'python3'

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
        UnittestOptions.pythonVersion = newPythonVersion

    @staticmethod
    def getPythonVersion():
        return UnittestOptions.pythonVersion


def getTestRootDir():
    return 'example'

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
    cmd.append(pythonVersion)
    cmd.append(execCommand)
    cmd.extend(commandList)
    cmd.extend(target.getProfileMapOptions())
    cmd.extend(target.getCompilerOptions())
    cmd.extend(target.getTargetOptions())
    cmd.extend(target.getRunTargetOptions())
    cmd.extend(target.getModeOptions())
    return execute(cmd, outputFile, errorFile)

def checkThatFileExists(path):
    fileExists = os.path.isfile(path)
    return fileExists

def checkThatDirExists(path):
    return os.path.isdir(path)

COMPILER_DEBUG_MODE = 'debug'
COMPILER_RELEASE_MODE = 'release'

class Compiler:
    def __init__(self, mode, name):
        self.mode = mode
        self.name = name

    def getName(self):
        return self.name

    def getMode(self):
        return self.mode

    def getOutputDirectory(self):
        return 'build/' + self.name + '/' + self.mode

    @staticmethod
    def getAllCompilers():
        return  [
                    Gcc(COMPILER_RELEASE_MODE), 
                    Gcc(COMPILER_DEBUG_MODE),
                    Clang(COMPILER_RELEASE_MODE),
                    Clang(COMPILER_DEBUG_MODE)
                ]

class Gcc(Compiler):
    def __init__(self, mode = 'release'):
        super(Gcc, self).__init__(mode, 'gcc')

class Clang(Compiler):
    def __init__(self, mode = 'release'):
        super(Clang, self).__init__(mode, 'clang')
