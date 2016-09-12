from .valgrind import *

class ValgrindMemcheck(Valgrind):
    def __init__(self):
       Valgrind.__init__(self, 'memcheck')
