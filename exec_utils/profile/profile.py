#!/usr/bin/python

from ..util.util import *
from ..runner.runner import runner
from ..filter.filterchain import FilterChain
from ..filter.perf import Perf
from ..filter.valgrindCallgrind import ValgrindCallgrind

def profileSystem(target, showStuff, options):
    mode = target.getMode()
    compiler = target.getCompiler()
    profileMethod = target.getProfileMethod()

    filterchain = FilterChain()
    if profileMethod == 'perf':
        filterchain.addFilter(Perf(target.getRootBuildDir(), showStuff))

    elif profileMethod == 'callgrind':
        filterchain.addFilter(ValgrindCallgrind(target.getRootBuildDir(), showStuff))

    else:
        print('Error: unknown method to profile project: ' + profileMethod)
        return False

    return runner(target, showStuff, options, filterchain)
