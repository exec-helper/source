#!/usr/bin/python

from .scons import Scons

def initBuildSystem(options, target):
    return options.getBuildSystem().init(options, target)

def buildBuildSystem(target, verbosity, options, prependCommand = None):
    return options.getBuildSystem().build(target, verbosity, options, prependCommand)

def cleanBuildSystem(options, target, verbose):
    return options.getBuildSystem().clean(options, target, verbose)

def distcleanBuildSystem(options, target):
    return options.getBuildSystem().distclean(options, target)
