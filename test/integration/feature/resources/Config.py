import os
import tempfile
import uuid

class Config(object):
    def __init__(self, directory = tempfile.gettempdir(), prefix = 'test-', suffix = '.exec-helper'):
        self._settings_file = directory + '/' + prefix + str(uuid.uuid4()) + suffix
        self._data = ""
        if os.path.exists(self._settings_file):
            raise AssertionError("Temporary file '{file}' already exists!".format(file = self._settings_file))

    def __del__(self):
        self.remove()

    @property
    def file(self):
        return self._settings_file

    def write(self):
        with open(self._settings_file, 'w') as f:
            f.write(self._data)

    def remove(self):
        if os.path.exists(self._settings_file):
            os.remove(self._settings_file)
