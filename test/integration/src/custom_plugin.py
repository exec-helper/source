import os

from command import Command

class CustomPlugin():
    _suffix = '.lua'

    def __init__(self, name, directory):
        self._id = name
        self._file = directory + '/' + self._id + self._suffix

        assert(os.path.isdir(directory))

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
        if os.path.exists(self._file):
            os.remove(self._file)
