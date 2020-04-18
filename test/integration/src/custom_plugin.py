import os
from pathlib import Path

from command import Command

class CustomPlugin():
    _suffix = '.lua'

    def __init__(self, name, directory):
        assert(directory.is_dir())

        self._id = name
        self._file = Path(directory).joinpath(self._id + self._suffix)

        self.write(self._id, self._file)

    def __del__(self):
        self.remove()
        pass

    @property
    def id(self):
        return self._id

    @staticmethod
    def write(id, file):
        with open(file, 'w') as f:
            f.write('task:add_args(get_commandline())\n')
            f.write('register_task(task)\n')

    def remove(self):
        self._file.unlink()
