#!/usr/bin/python

from ..util.util import *
from ..runner.runner import runner
from ..filter.filterchain import FilterChain
from ..filter.perf import Perf
from ..filter.valgrindCallgrind import ValgrindCallgrind

def profileSystem(target, mode, runTarget, compiler, profileMethod, showStuff, options):
    filterchain = FilterChain()
    if profileMethod == 'perf':
        filterchain.addFilter(Perf(getBuildDir(mode, compiler), showStuff))

    elif profileMethod == 'callgrind':
        filterchain.addFilter(ValgrindCallgrind(getBuildDir(mode, compiler), showStuff))

    else:
        print('Error: unknown method to analyze project: ' + method)
        return False

    return runner(target, mode, runTarget, compiler, showStuff, options, filterchain)
