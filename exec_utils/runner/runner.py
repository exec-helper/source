#!/usr/bin/python

from ..util.util import *
from ..filter.filterchain import FilterChain
from ..filter.runUnitTest import RunUnittest
from ..filter.runPerformanceTest import RunPerformancetest

def run(target, mode, runTarget, compiler, showStuff, filterchain = None):
    if filterchain is None:
        filterchain = FilterChain()

    buildDir = getBuildDir(mode, compiler)
    filterchain.addFilter(RunTest(target, mode, buildDir))
    return filterchain.execute()

def runner(target, mode, runTarget, compiler, showStuff, filterchain = None):
    return run(target, mode, runTarget, compiler, showStuff, filterchain)
