from ..util.util import *

class FilterChain:
    def __init__(self):
        self.filters = []

    def addFilter(self, filter):
        self.filters.append(filter)

    def execute(self):
        if not self.filters:
            print("Warning: no filters added to this filter chain.")
            return True

        self.preProcess()
        cmd = self.process()
        retCode = isSuccess(executeInShell(cmd))
        self.postProcess()
        return retCode

    def preProcess(self):
        for filter in self.filters:
            filter.preProcess()

    def process(self):
        cmd = []
        for filter in self.filters:
            cmd = filter.process(cmd)
        return cmd

    def postProcess(self):
        for filter in self.filters:
            filter.postProcess()
