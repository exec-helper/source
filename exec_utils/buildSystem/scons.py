#!/usr/bin/python

import shutil

from ..util.util import *

class Scons:
    @staticmethod
    def getBuildCommand():
        command = ['scons']
        return command

    def __init__(self):
        pass

    def init(self, workingDir, mode):
        return True

    def build(self, target, verbose, options, prependCommand = None):
        if prependCommand is None:
            build_command = self.getBuildCommand()
        else:
            build_command = prependCommand
            build_command.extend(self.getBuildCommand())

        mode = target.getMode()
        build_command.append('mode={0}'.format(mode))

        # Add multithreaded building by default
        if not target.getBuildSingleThreaded():
            build_command.append('-j8')

        build_command.append('compiler=' + target.getCompiler()) 
        toolchainPath = target.getToolchainPath()
        if toolchainPath is not None and toolchainPath != '':
            build_command.append('toolchainPath=' + toolchainPath)

        targetName = target.getTargetName()
        profile = target.getProfile()
        if(not targetName or targetName == 'all'):
            print("Building default targets in {0} mode".format(mode))
        else:
            targetName = targetName + profile.suffix
            print("Building target {0} in {1} mode".format(targetName, mode))
            build_command.append(targetName)

        if verbose:
            build_command.extend(['--debug=explain'])

        return isSuccess(executeInShell(build_command))

    def clean(self, target, mode, compiler, verbose):
        clean_command = self.getBuildCommand()
        clean_command.append('--clean')
        clean_command.append('compiler=' + compiler) 

        if(target and target != 'all'):
            clean_command.append(target)
        else:
            target = 'all'
        clean_command.append(mode)

        if verbose:
            pass

        print("Cleaning {0}".format(target))
        return isSuccess(executeInShell(clean_command))

    def distclean(self, options, target):
        mode = target.getMode()
        compiler = target.getCompiler()
        buildDir = getBuildDir(mode, compiler)
        print("Dist cleaning {0}".format(buildDir))

        if exists(buildDir):
            shutil.rmtree(buildDir)

        binDir = getBinDir(mode)
        print("Dist cleaning {0}".format(binDir))
        if exists(binDir):
            shutil.rmtree(binDir)
        return True
