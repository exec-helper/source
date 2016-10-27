#!/usr/bin/python

# Note: this code is not (yet) generic nor production ready

import shutil

from ..util.util import *
from ..target.target import *

class Make:
    @staticmethod
    def getBuildCommand():
        command = ['make']
        return command

    def __init__(self):
        pass

    def init(self, options, target):
        return True

    def build(self, target, verbose, options, prependCommand = None):
        targetName = target.getTargetName()
        mode = target.getMode()
        profile = target.getProfile()
        buildDir = target.getRootBuildDir()

        if prependCommand is None:
            build_command = self.getBuildCommand()
        else:
            build_command = prependCommand
            build_command.extend(self.getBuildCommand())

        # Add multithreaded building by default
        if not target.getBuildSingleThreaded():
            build_command.append('-j8')

        if(not targetName or targetName == 'default'):
            print("Building default targetNames in {0} mode".format(mode))
            build_command.extend(['-C', buildDir]) 
        else:
            targetName = targetName + profile.suffix
            build_command.extend(['-C', buildDir + '/' + targetName]) 
            print("Building targetName {0} in {1} mode".format(targetName, mode))

        if verbose:
            build_command.extend(['V=1'])

        return isSuccess(executeInShell(build_command))

    def clean(self, options, target, verbose):
        targetName = target.getTargetName()
        distribution = target.getDistribution()
        architecture = target.getArchitecture()
        buildDir = target.getRootBuildDir()

        clean_command = self.getBuildCommand()
        clean_command.append('clean')

        if(targetName and targetName != 'all'):
            cleanTarget = buildDir + '/' + targetName
        else:
            cleanTarget = buildDir

        clean_command.extend(['-C', cleanTarget]) 

        if verbose:
            pass

        print("Cleaning {0}".format(cleanTarget))
        return isSuccess(executeInShell(clean_command))

    def distclean(self, options, target):
        mode = target.getMode()
        buildDir = target.getRootBuildDir()
        print("Dist cleaning {0}".format(buildDir))

        if exists(buildDir):
            shutil.rmtree(buildDir)

        binDir = getBinDir(mode)
        print("Dist cleaning {0}".format(binDir))
        if exists(binDir):
            shutil.rmtree(binDir)
        return True
