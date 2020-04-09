import tempfile

class WorkingDir(object):
    def __init__(self):
        self._dir = tempfile.gettempdir()

    @property
    def dir(self):
        return self._dir
