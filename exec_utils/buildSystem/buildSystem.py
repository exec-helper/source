#!/usr/bin/python

from .scons import Scons

buildSystem = Scons()

def initBuildSystem(workingDir, mode):
    return buildSystem.init(workingDir, mode)

def buildBuildSystem(target, mode, profile, compiler, toolchainPath, verbose, singleThreaded, options, prependCommand = None):
    return buildSystem.build(target, mode, profile, compiler, toolchainPath, verbose, singleThreaded, options, prependCommand)

def cleanBuildSystem(target, mode, compiler, verbose):
    return buildSystem.clean(target, mode, compiler, verbose)

def distcleanBuildSystem(mode, compiler):
    return buildSystem.distclean(mode, compiler)
