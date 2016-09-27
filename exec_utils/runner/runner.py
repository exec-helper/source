#!/usr/bin/python

from ..util.util import *
from ..filter.filterchain import FilterChain
from ..filter.runTest import RunTest

def run(target, mode, runTarget, compiler, showStuff, options, filterchain = None):
    if filterchain is None:
        filterchain = FilterChain()

    target_to_run = target + getRunTargetSuffix(options.getProfileMap(), runTarget)

    buildDir = getBuildDir(mode, compiler)
    directoryToRun = buildDir + '/' + getRunTargetRunDir(options.getProfileMap(), runTarget)
    filterchain.addFilter(RunTest(target_to_run, mode, directoryToRun))
    return filterchain.execute()

def runner(target, mode, runTarget, compiler, showStuff, options, filterchain = None):
    print('Running target: ' + runTarget)
    return run(target, mode, runTarget, compiler, showStuff, options, filterchain)
