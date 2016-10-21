#!/usr/bin/python

from ..util.util import executeInShell
from ..vcsSystem.vcsSystem import *
from ..buildSystem.buildSystem import *
from ..runner.runner import *
from ..analyze.analyze import *
from ..profile.profile import *
from exec_utils.options.options import Options

def init(workingDir, options):
    return initVcs() and initBuildSystem(options, workingDir, options.getModes())

def build(options):
    for mode in options.getModes():
        for target in options.getTargets():
            for profile in options.getProfiles(options.getRunTargets()):
                for compiler in options.getCompilers():
                    if not buildBuildSystem(target, mode, profile, compiler, options.getToolchainPath(), options.getVerbosity(), options.getBuildSingleThreaded(), options):
                        return False
    return True

def clean(options):
    for mode in options.getModes():
        for target in options.getTargets():
            for compiler in options.getCompilers():
                if not cleanBuildSystem(options, target, mode, compiler, options.getVerbosity()):
                    return False
    return True

def distclean(options):
    # TODO: Use python to clean python files
    for mode in options.getModes():
        for compiler in options.getCompilers():
            if not distcleanBuildSystem(options, mode, compiler):
                return False
    return True

def run(options):
    for mode in options.getModes():
        for target in options.getTargets():
            for profile in options.getProfiles(options.getRunTargets()):
                for compiler in options.getCompilers():
                    if not runner(target, mode, profile, compiler, options.getShowStuff(), options):
                        return False
    return True

def analyze(options):
    for mode in options.getModes():
        for analyzeMethod in options.getAnalyzeMethods():
            for profile in options.getProfiles(options.getRunTargets()):
                for target in options.getTargets():
                    if not analyzeBuildSystem(analyzeMethod, mode, target, profile, options.getVerbosity(), options.getShowStuff(), options):
                        return False
    return True

def profile(options):
    for mode in options.getModes():
        for profileMethod in options.getProfileMethods():
            for target in options.getTargets():
                for profile in options.getProfiles(options.getRunTargets()):
                    for compiler in options.getCompilers():
                        if not profileSystem(target, mode, profile, compiler, profileMethod, options.getShowStuff(), options):
                            return False
    return True

