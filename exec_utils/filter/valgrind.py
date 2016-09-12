from .filter import *

class Valgrind(Filter):
    def __init__(self, tool):
        self.tool = tool

    def process(self, cmd):
        return cmd + ['valgrind', '--tool=' + self.tool]
