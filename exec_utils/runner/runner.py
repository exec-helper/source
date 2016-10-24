#!/usr/bin/python

from ..util.util import *
from ..filter.filterchain import FilterChain
from ..filter.runTest import RunTest
from ..target.target import Target

def run(target, showStuff, options, filterchain = None):
    if filterchain is None:
        filterchain = FilterChain()

    profile = target.getProfile()
    mode = target.getMode()
    compiler = target.getCompiler()
    target_to_run = target.getTargetName() + profile.suffix

    print('Running target: ' + target_to_run)

    buildDir = getBuildDir(mode, compiler)
    directoryToRun = buildDir + '/' + profile.targetDirectory
    filterchain.addFilter(RunTest(target_to_run, mode, directoryToRun))
    return filterchain.execute()

def runner(target, showStuff, options, filterchain = None):
    return run(target, showStuff, options, filterchain)
