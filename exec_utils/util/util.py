#!/usr/bin/python

import os
import sys
import subprocess
import glob
import fnmatch
import importlib

BIN_DIR = 'bin'
SRC_DIR = 'src'
EXIT_SUCCESS = 0

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def coloredOutput(output, color):
    return color + output + bcolors.ENDC

def info(outputString):
    print(outputString)

def error(outputString):
    print(coloredOutput(outputString, bcolors.FAIL))

def warning(outputString):
    print(coloredOutput(outputString, bcolors.WARNING))

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

def getBinDir(mode):
    return BIN_DIR + '/' + mode

def getAllTargets(dir):
    targets = []
    targets.extend(getAllRealTargets(dir))
    targets.extend(getAllPhonyTargets())
    return targets

def getSrcDir(target = None):
    return SRC_DIR

def getRunTargets(profiles, returnAll = True):
    runTargets = profiles.keys()
    if returnAll:
        runTargets.append('all')
    return runTargets

def getAnalyzeMethods(returnAll = True):
    analyzeMethods = ['clang', 'cppcheck', 'cpd', 'valgrind']
    if returnAll:
        analyzeMethods.append('all')
        analyzeMethods.append('simian')     # Not officially supported
        analyzeMethods.append('cpplint')    # Not officially supported
    return analyzeMethods

def executeInShell(cmd, working_directory = '.'):
    pwd()
    print(coloredOutput("\nExecuting '{0}' in '{1}'".format(listToString(cmd, ' '), working_directory), bcolors.OKBLUE))
    return subprocess.call(cmd, cwd = working_directory)

def getShellOutput(cmd, working_directory = '.'):
    print(coloredOutput("\nExecuting '{0}' in '{1}'".format(listToString(cmd, ' '), working_directory), bcolors.OKBLUE))
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

def replaceWith(hayStack, needle, replacementNeedle):
    while hayStack.count(needle) > 0:
        index = hayStack.index(needle)
        hayStack.pop(index)
        insertIndex = index
        for replacement in replacementNeedle:
            hayStack.insert(insertIndex, replacement) 
            insertIndex = insertIndex + 1
    return hayStack


