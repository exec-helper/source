#!/usr/bin/python

from .scons import Scons

def initBuildSystem(options, workingDir, mode):
    return options.getBuildSystem().init(workingDir, mode)

def buildBuildSystem(target, verbosity, options, prependCommand = None):
    return options.getBuildSystem().build(target, verbosity, options, prependCommand)

def cleanBuildSystem(options, target, mode, compiler, verbose):
    return options.getBuildSystem().clean(target, mode, compiler, verbose)

def distcleanBuildSystem(options, target):
    return options.getBuildSystem().distclean(options, target)
