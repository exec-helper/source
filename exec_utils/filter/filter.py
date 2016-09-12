# Base class for all filters: steps not required for a specific filter can be ommitted
class Filter:
    def __init__(self):
        pass

    def preProcess(self):
        pass

    def process(self, cmd):
        return cmd
    
    def postProcess(self):
        pass
