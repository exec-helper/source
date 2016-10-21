#!/usr/bin/python

# Note: this code is not (yet) generic nor production ready

import shutil

from ..util.util import *

class Make:
    @staticmethod
    def getBuildCommand():
        command = ['make']
        return command

    def __init__(self):
        pass

    def init(self, workingDir, mode):
        return True

    def build(self, target, mode, profile, compiler, toolchainPath, verbose, singleThreaded, options, prependCommand = None):
        if prependCommand is None:
            build_command = self.getBuildCommand()
        else:
            build_command = prependCommand
            build_command.extend(self.getBuildCommand())

        # Add multithreaded building by default
        if not singleThreaded:
            build_command.append('-j8')

        if(not target or target == 'default'):
            print("Building default targets in {0} mode".format(mode))
            build_command.extend(['-C', 'build/wheezy/i386']) 
        else:
            target = target + profile.suffix
            build_command.extend(['-C', 'build/wheezy/i386/' + target]) 
            print("Building target {0} in {1} mode".format(target, mode))

        if verbose:
            build_command.extend(['V=1'])

        return isSuccess(executeInShell(build_command))

    def clean(self, target, mode, compiler, verbose):
        clean_command = self.getBuildCommand()
        clean_command.append('clean')

        if(target and target != 'all'):
            clean_command.extend(['-C', 'build/wheezy/i386/' + target]) 
        else:
            clean_command.extend(['-C', 'build/wheezy/i386']) 

        if verbose:
            pass

        print("Cleaning {0}".format(target))
        return isSuccess(executeInShell(clean_command))

    def distclean(self, mode, compiler):
        buildDir = 'build/wheezy/i386'
        print("Dist cleaning {0}".format(buildDir))

        if exists(buildDir):
            shutil.rmtree(buildDir)

        binDir = getBinDir(mode)
        print("Dist cleaning {0}".format(binDir))
        if exists(binDir):
            shutil.rmtree(binDir)
        return True
