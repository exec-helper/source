import os
import tempfile
import uuid
import yaml

import command

class Config(object):
    _prefix = 'test-'
    _suffix = '.exec-helper'

    def __init__(self, directory = tempfile.gettempdir()):
        self._settings_file = directory + '/' + self._prefix + str(uuid.uuid4()) + self._suffix
        self._commands = dict()
        if os.path.exists(self._settings_file):
            raise AssertionError("Temporary file '{file}' already exists!".format(file = self._settings_file))

    def __del__(self):
        self.remove()

    @property
    def file(self):
        return self._settings_file

    @property
    def commands(self):
        return self._commands

    def create_command(self, cmd):
        self._commands[cmd] = command.Command(cmd)

    def set_environment(self, cmd, envs):
        self._commands[cmd].set_environment(envs)

    def write(self):
        config_file = dict()
        for id,cmd in self._commands.items():
            if not 'commands' in config_file:
                config_file['commands'] = []
            config_file['commands'].append(id)

            config_file.update(cmd.to_dict())
            cmd.write_binary()

        with open(self._settings_file, 'w') as f:
            yaml.dump(config_file, f)

    def remove(self):
        if os.path.exists(self._settings_file):
            os.remove(self._settings_file)
