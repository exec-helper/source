# Base class for filters that want to run a test binary
from ..util.util import *
from .filter import *

class RunTest(Filter):
    def __init__(self, target, mode, buildDir, suffix = ''):
        if(target == 'all'):
            print("Error: this filter can not run 'all' targets")
        
        self.target = target
        self.mode = mode
        self.buildDir = buildDir
        self.suffix = suffix

    def process(self, cmd):
        return cmd + [self.buildDir + '/' + self.target + self.suffix]
