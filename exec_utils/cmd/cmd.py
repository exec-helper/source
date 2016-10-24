#!/usr/bin/python

from ..util.util import executeInShell
from ..vcsSystem.vcsSystem import *
from ..buildSystem.buildSystem import *
from ..runner.runner import *
from ..analyze.analyze import *
from ..profile.profile import *
from exec_utils.options.options import Options

def init(options, target):
    if not initVcs():
        return False

    for initTarget in target.iterateTargets('mode', 'compiler'):
        if not initBuildSystem(options, initTarget):
            return False
    return True

def build(options, target):
    for buildTarget in target.iterateTargets('mode', 'compiler', 'target', 'runTarget'):
        if not buildBuildSystem(buildTarget, options.getVerbosity(), options):
            return False
    return True

def clean(options, target):
    for cleanTarget in target.iterateTargets('mode', 'compiler', 'target'):
        if not cleanBuildSystem(options, cleanTarget, options.getVerbosity()):
            return False
    return True

def distclean(options, target):
    # TODO: Use python to clean python files
    for distcleanTarget in target.iterateTargets('mode', 'compiler'):
        if not distcleanBuildSystem(options, distcleanTarget):
            return False
    return True

def run(options, target):
    for runTarget in target.iterateTargets('mode', 'compiler', 'target', 'runTarget'):
        if not runner(runTarget, options.getShowStuff(), options):
            return False
    return True

def analyze(options, target):
    for analyzeTarget in target.iterateTargets('analyzeMethod', 'mode', 'compiler', 'target', 'runTarget'):
        if not analyzeBuildSystem(analyzeTarget, options.getVerbosity(), options.getShowStuff(), options):
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

