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

    for initTarget in target.iterateTargets('distribution', 'architecture', 'compiler', 'mode'):
        if not initBuildSystem(options, initTarget):
            return False
    return True

def build(options, target):
    for buildTarget in target.iterateTargets('distribution', 'architecture', 'compiler', 'mode', 'target', 'runTarget'):
        if not buildBuildSystem(buildTarget, options.getVerbosity(), options):
            return False
    return True

def clean(options, target):
    for cleanTarget in target.iterateTargets('distribution', 'architecture', 'compiler', 'mode', 'target'):
        if not cleanBuildSystem(options, cleanTarget, options.getVerbosity()):
            return False
    return True

def distclean(options, target):
    # TODO: Use python to clean python files
    for distcleanTarget in target.iterateTargets('distribution', 'architecture', 'compiler', 'mode'):
        if not distcleanBuildSystem(options, distcleanTarget):
            return False
    return True

def run(options, target):
    for runTarget in target.iterateTargets('distribution', 'architecture', 'compiler', 'mode', 'target', 'runTarget'):
        if not runner(runTarget, options.getShowStuff(), options):
            return False
    return True

def analyze(options, target):
    for analyzeTarget in target.iterateTargets('analyzeMethod', 'distribution', 'architecture', 'compiler', 'mode', 'target', 'runTarget'):
        if not analyzeBuildSystem(analyzeTarget, options.getVerbosity(), options.getShowStuff(), options):
            return False
    return True

def profile(options, target):
    for profileTarget in target.iterateTargets('profileMethod', 'distribution', 'architecture', 'compiler', 'mode', 'target', 'runTarget'):
        if not profileSystem(profileTarget, options.getShowStuff(), options):
            return False
    return True

