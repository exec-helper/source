from ..util.util import *
from .runTest import RunTest

# Class for running unit test targets
class RunUnittest(RunTest):
    def __init__(self, target, mode, compiler):
        buildDir = self.getUnittestDir(mode, compiler)
        RunTest.__init__(self, target, mode, buildDir, '-unittest')

    @staticmethod
    def getUnittestDir(mode, compiler):
        return getBuildDir(mode, compiler) + '/bin/test/unittest'
