from ..util.util import *
from .runTest import RunTest

# Class for running performance test targets
class RunPerformancetest(RunTest):
    def __init__(self, target, mode, compiler):
        buildDir = self.getUnittestDir(mode, compiler)
        RunTest.__init__(self, target, mode, buildDir, '-benchmark')

    @staticmethod
    def getUnittestDir(mode, compiler):
        return getBuildDir(mode, compiler) + '/bin/benchmark'
