from ..util.util import *
from .filter import *

class Perf(Filter):
    def __init__(self, buildDir, showReport):
        self.outputFile = buildDir + '/../perf.data'
        self.showReport = showReport

    def process(self, cmd):
        return cmd + ['perf', 'record', '-o', self.outputFile]

    def postProcess(self):
        if self.showReport:
            cmd = ['perf', 'report', '-i', self.outputFile] 
            executeInShell(cmd)
