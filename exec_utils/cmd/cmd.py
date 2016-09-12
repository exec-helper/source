#!/usr/bin/python

from ..util.util import executeInShell
from ..vcsSystem.vcsSystem import *
from ..buildSystem.buildSystem import *
from ..runner.runner import *
from ..analyze.analyze import *
from ..profile.profile import *
from exec_utils.options.options import Options

def init(workingDir, options):
    return initVcs() and initBuildSystem(workingDir, options.getModes())

def build(options):
    for target in options.getTargets():
        for runMode in options.getRunTargets():
            for compiler in options.getCompilers():
                if not buildBuildSystem(target, options.getModes(), runMode, compiler, options.getToolchainPath(), options.getVerbosity(), options.getBuildSingleThreaded()):
                    return False
    return True

def clean(options):
    for target in options.getTargets():
        for compiler in options.getCompilers():
            if not cleanBuildSystem(target, options.getModes(), compiler, options.getVerbosity()):
                return False
    return True

def distclean(options):
    # TODO: Use python to clean python files
    for compiler in options.getCompilers():
        if not distcleanBuildSystem(options.getModes(), compiler):
            return False
    return True

def run(options):
    for target in options.getTargets():
        for runTarget in options.getRunTargets():
            for compiler in options.getCompilers():
                if not runner(target, options.getModes(), runTarget, compiler, options.getShowStuff()):
                    return False
    return True

def analyze(options):
    for analyzeMethod in options.getAnalyzeMethods():
        for target in options.getTargets():
            if not analyzeBuildSystem(analyzeMethod, options.getModes(), target, options.getVerbosity(), options.getShowStuff(), options):
                return False
    return True

def profile(options):
    for profileMethod in options.getProfileMethods():
        for target in options.getTargets():
            for runTarget in options.getRunTargets():
                for compiler in options.getCompilers():
                    if not profileSystem(target, options.getModes(), runTarget, compiler, profileMethod, options.getShowStuff(), options):
                        return False
    return True

