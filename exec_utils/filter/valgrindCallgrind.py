from ..util.util import *
from .valgrind import *

class ValgrindCallgrind(Valgrind):
    def __init__(self, buildDir, showReport):
        Valgrind.__init__(self, 'callgrind')
        self.outputFile = buildDir + '/../callgrind.out'
        self.showReport = showReport

    def process(self, cmd):
        cmd = Valgrind.process(self, cmd)
        cmd.append('--callgrind-out-file=' + self.outputFile)
        return cmd

    def postProcess(self):
        if self.showReport:
            cmd = ['kcachegrind', self.outputFile] 
            executeInShell(cmd)
