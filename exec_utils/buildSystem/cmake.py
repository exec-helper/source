#!/usr/bin/python

import shutil

from ..util.util import *

class Cmake:
    def __init__(self):
        pass

    def init(self, workingDir, mode):
        buildPath = getBuildDir(mode)
        print("Initializing '{0}' build directory".format(buildPath))
        createDirIfNotExists(buildPath)

        modeFlag = "-DCMAKE_BUILD_TYPE=" + mode
        goToDir(buildPath)
        retValue = executeInShell(["cmake", modeFlag, "../.."])
        goToDir(workingDir)
        return isSuccess(retValue)

    def build(self, target, mode, verbose):
        build_command = ['make']
        build_command.append('install')

        buildDir = getBuildDir(mode)

        if(not target or target == 'all'):
            print("Building all targets in {0} mode".format(mode))
        else:
            print("Building target {0} in {1} mode".format(target, mode))
            buildDir = getBuildDir(mode) + '/' + target

        build_command.extend(['-C', buildDir])

        if verbose:
            build_command.append('VERBOSE=1')

        return isSuccess(executeInShell(build_command))

    def clean(self, target, mode, verbose):
        clean_command = ['make']
        clean_command.append('clean')

        buildDir = getBuildDir(mode)
        if(target and target != 'all'):
            buildDir = getBuildDir(mode) + '/' + target
        clean_command.extend(['-C', buildDir])

        if verbose:
            clean_command.append('VERBOSE=1')

        print("Cleaning {0}".format(buildDir))
        return isSuccess(executeInShell(clean_command))

    def distclean(self, mode):
        buildDir = getBuildDir(mode)
        print("Dist cleaning {0}".format(buildDir))
        if exists(buildDir):
            shutil.rmtree(buildDir)
        return True
