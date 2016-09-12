#!/usr/bin/python

import shutil

from ..util.util import *

class B2:
    @staticmethod
    def getB2Dir():
        return '3rdparty/boost/build'

    @staticmethod
    def addBuildRoot(command):
        command.append('-sBOOST_ROOT={0}'.format(B2.getB2Dir()))

    @staticmethod
    def getBuildCommand():
        command = ['3rdparty/boost/build/b2']
        B2.addBuildRoot(command)
        return command

    def __init__(self):
        pass

    def init(self, workingDir, mode):
        command_working_dir = B2.getB2Dir()
        
        init_command = ['./bootstrap.sh']
        return isSuccess(executeInShell(init_command, command_working_dir))

    def build(self, target, mode, verbose):
        build_command = self.getBuildCommand()
        build_command.append('variant={0}'.format(mode))

        if(not target or target == 'all'):
            print("Building all targets in {0} mode".format(mode))
        else:
            print("Building target {0} in {1} mode".format(target, mode))
            build_command.append(target)

        # Add multithreaded building by default
        build_command.append('-j8')

        if verbose:
            build_command.extend(['-d', '2'])

        return isSuccess(executeInShell(build_command))

    def clean(self, target, mode, verbose):
        clean_command = self.getBuildCommand()
        clean_command.append('--clean')

        if(target and target != 'all'):
            clean_command.append(target)
        else:
            target = 'all'
        clean_command.append(mode)

        if verbose:
            clean_command.extend(['-d', '2'])

        print("Cleaning {0}".format(target))
        return isSuccess(executeInShell(clean_command))

    def distclean(self, mode):
        buildDir = getBuildDirWithoutMode()
        print("Dist cleaning {0}".format(buildDir))

        if exists(buildDir):
            shutil.rmtree(buildDir)

        binDir = getBinDir(mode)
        print("Dist cleaning {0}".format(binDir))
        if exists(binDir):
            shutil.rmtree(binDir)
        return True
