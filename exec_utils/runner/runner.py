#!/usr/bin/python

from ..util.util import *
from ..filter.filterchain import FilterChain
from ..filter.runTest import RunTest

def run(target, mode, profile, compiler, showStuff, options, filterchain = None):
    if filterchain is None:
        filterchain = FilterChain()

    target_to_run = target + profile.suffix

    buildDir = getBuildDir(mode, compiler)
    directoryToRun = buildDir + '/' + profile.targetDirectory
    filterchain.addFilter(RunTest(target_to_run, mode, directoryToRun))
    return filterchain.execute()

def runner(target, mode, profile, compiler, showStuff, options, filterchain = None):
    print('Running target: ' + profile.id)
    return run(target, mode, profile, compiler, showStuff, options, filterchain)
