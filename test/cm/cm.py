from exec_utils.util.util import *

import os

SHELL_RETURN_CODE = 0

PYTHON2 = 'python2'
PYTHON3 = 'python3'

def getAllTargets():
    return ['src/example', 'bin/example',
            'src/example-unittest', 'bin/test/unittest/example-unittest']

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

def getProfileMap(profileMap = 'exec-helper_profiles'):
    if profileMap is None:
        return []
    else:
        return ['--profile-map', profileMap]

def getDistcleanCommand(profileMap = 'exec-helper_profiles'):
    cmd = ['distclean']
    cmd.extend(getProfileMap())
    return cmd

def getCleanCommand(profileMap = 'exec-helper_profiles'):
    cmd = ['clean']
    cmd.extend(getProfileMap())
    return cmd

def getBuildCommand(profileMap = 'exec-helper_profiles'):
    cmd = ['build']
    cmd.extend(getProfileMap())
    return cmd

def execute(cmd, outputFile = None, errorFile = None):
    retCode,output,error = getShellOutputAndReturnCode(cmd, getTestRootDir(), False)
    if outputFile:
        writeFile(outputFile, output)

    if errorFile:
        writeFile(errorFile, error)
    return retCode == SHELL_RETURN_CODE

def checkThatFileIsExists(path):
    fileExists = os.path.isfile(path)
    return fileExists

def checkThatDirExists(path):
    return os.path.isdir(path)

class Compiler:
    def __init__(self, mode, name):
        self.mode = mode
        self.name = name

    def getCommandLineSpecification(self):
        cmd = []
        cmd.extend(['--compiler', self.name])
        cmd.extend(['--mode', self.mode])
        return cmd
    
    def getOutputDirectory(self):
        return 'build/' + self.name + '/' + self.mode

class Gcc(Compiler):
    def __init__(self, mode = 'release'):
        super(Gcc, self).__init__(mode, 'gcc')

class Clang(Compiler):
    def __init__(self, mode = 'release'):
        super(Clang, self).__init__(mode, 'clang')
