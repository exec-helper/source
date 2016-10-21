#!/usr/bin/python

from .scons import Scons

def initBuildSystem(options, workingDir, mode):
    return options.getBuildSystem().init(workingDir, mode)

def buildBuildSystem(target, mode, profile, compiler, toolchainPath, verbose, singleThreaded, options, prependCommand = None):
    return options.getBuildSystem().build(target, mode, profile, compiler, toolchainPath, verbose, singleThreaded, options, prependCommand)

def cleanBuildSystem(options, target, mode, compiler, verbose):
    return options.getBuildSystem().clean(target, mode, compiler, verbose)

def distcleanBuildSystem(options, mode, compiler):
    return options.getBuildSystem().distclean(mode, compiler)
