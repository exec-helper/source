#!/usr/bin/python

import os
import sys
import subprocess
import glob
import fnmatch
import importlib

BUILD_DIR = 'build'
BIN_DIR = 'bin'
SRC_DIR = 'src'
EXIT_SUCCESS = 0

def listToString(list, separator):
    return separator.join(list)

def getCurrentDir():
    return os.getcwd()

def exists(path):
    return os.path.exists(path)

def createDirIfNotExists(path):
    if not exists(path):
        os.makedirs(path)

def goToDir(path):
    os.chdir(path)

def pwd():
    print("Current working dir: {0}".format(getCurrentDir()))

def getBuildDirWithoutMode():
    return BUILD_DIR

def getBuildDir(mode, compiler):
    return BUILD_DIR + '/' + compiler + '/' + mode

def getBinDir(mode):
    return BIN_DIR + '/' + mode

def getAllTargets(dir):
    targets = []
    targets.extend(getAllRealTargets(dir))
    targets.extend(getAllPhonyTargets())
    return targets

def getTargets(dir, returnAll = True):
    targets = ['example']
    if returnAll:
        targets.append('all')
    return targets

def getSrcDir(target = None):
    return SRC_DIR

def getRunTargets(profileMapFile = None, returnAll = True):
    runTargets = getProfileMap(profileMapFile).keys()
    if returnAll:
        runTargets.append('all')
    return runTargets

def getRunTargetSuffix(profileMapFile, runTarget):
    return getProfileMapParameter(profileMapFile, runTarget, 0)

def getRunTargetRunDir(profileMapFile, runTarget):
    return getProfileMapParameter(profileMapFile, runTarget, 1)

def getProfileMapParameter(profileMapFile, runTarget, index):
    profileMap = getProfileMap(profileMapFile)
    print(str(profileMap))
    if runTarget in profileMap:
        if len(profileMap) > index:
            return getProfileMap(profileMapFile)[runTarget][index]
        else:
            raise IndexError
    else:
        raise KeyError

def getProfileMap(profileMapFile):
    try:
        profiles = importlib.import_module(profileMapFile)
        return profiles.profileMap
    except ImportError:
        # Means it may be python 2
        try:
            module,filename = os.path.splitext(profileMapFile)
            sys.path.append(module)
            filename = filename.lstrip('.')
            profiles = importlib.import_module(filename)
            print(profiles)
            return profiles.profileMap
        except ImportError:
            pass
        except TypeError:
            pass
    except TypeError:
        pass
    return {}

def getAnalyzeMethods(returnAll = True):
    analyzeMethods = ['clang', 'cppcheck', 'cpd', 'valgrind']
    if returnAll:
        analyzeMethods.append('all')
        analyzeMethods.append('simian')     # Not officially supported
        analyzeMethods.append('cpplint')    # Not officially supported
    return analyzeMethods

def executeInShell(cmd, working_directory = '.'):
    pwd()
    print("\nExecuting '{0}' in '{1}'".format(listToString(cmd, ' '), working_directory))
    return subprocess.call(cmd, cwd = working_directory)

def getShellOutput(cmd, working_directory = '.'):
    print("\nExecuting '{0}' in '{1}'".format(listToString(cmd, ' '), working_directory))
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, cwd = working_directory)
    out, err = process.communicate()
    return out,err

def getShellOutputAndReturnCode(cmd, working_directory = '.', verbose = True):
    if verbose:
        print("\nExecuting '{0}' in '{1}'".format(listToString(cmd, ' '), working_directory))
    process = subprocess.Popen(cmd, stdout=subprocess.PIPE, cwd = working_directory)
    out, err = process.communicate()
    retCode = process.returncode
    return retCode,out,err

def isInstalled(binary):
    cmd = ['which', binary]
    return isSuccess(executeInShell(cmd))

def getAllDirs(path):
    dirs = [x[0] for x in os.walk(path)]
    for root, dirs, files in os.walk(path):
        return dirs

def getAllFiles(path):
    dirs = [x[0] for x in os.walk(path)]
    for root, dirs, files in os.walk(path):
        return files

def getAllFilesRecursive(rootDir, includePath=False):
    allFiles = []
    for root, dirs, files in os.walk(rootDir):
        for file in files:
            if includePath:
                allFiles.append(os.path.join(root,file))
            else:
                allFiles.append(file)
    return allFiles

def findAllFilesPattern(rootDir, pattern):
    try:
        # TODO: this is not backwards compatible with python 2.x until python 3.4
        return glob.glob(rootDir + '/**/*' + pattern, recursive=True)
    except TypeError:
        fileList = []
        files = getAllFilesRecursive(rootDir, True)
        for file in files:
            if fnmatch.fnmatch(file, pattern):
                fileList.append(file)

        return fileList

def getAllDirsThatContainPattern(rootDir, extension):
    uniqueDirs = set()
    for root, dirs, files in os.walk(rootDir):
        for file in files:
            if fnmatch.fnmatch(file, extension):
                uniqueDirs.add(root)
    return list(uniqueDirs)

def isSuccess(retValue):
    return retValue == EXIT_SUCCESS
