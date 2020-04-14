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
        self._patterns = set()
        self._plugin_search_path = []
        if os.path.exists(self._settings_file):
            raise AssertionError("Temporary file '{file}' already exists!".format(file = self._settings_file))

    def __del__(self):
        self.remove()
        pass

    @property
    def file(self):
        return self._settings_file

    @property
    def commands(self):
        return self._commands

    def create_command(self, command_id):
        """ Creates a command for the given command id using an implementation-specific plugin
        """
        self._commands[command_id] = command.Command(command_id, 'command-line-command')

    def add_command(self, command_id, command):
        """ Adds the given command as a command associated with the command id to the configuration """
        self._commands[command_id] = command

    def set_environment(self, cmd, envs):
        self._commands[cmd].set_environment(envs)

    def add_pattern(self, pattern):
        self._patterns.add(pattern)

    def add_plugin_search_path(self, path):
        self._plugin_search_path.append(path)

    def write(self):
        config_file = dict()

        # Make sure the config file is not empty
        config_file['blaat'] = []

        if self._plugin_search_path:
            config_file['additional-search-paths'] = self._plugin_search_path

        if self._patterns:
            config_file['patterns'] = {}
            for pattern in self._patterns:
                config_file['patterns'][pattern.id] = {
                    'default-values': pattern.default_values
                }
                if pattern.long_options:
                    config_file['patterns'][pattern.id]['long-option'] = pattern.long_options

        if self._commands:
            config_file['commands'] = []

        for id,cmd in self._commands.items():
            config_file['commands'].append(id)

            config_file.update(cmd.to_dict())
            cmd.write_binary()

        with open(self._settings_file, 'w') as f:
            yaml.dump(config_file, f)

    def remove(self):
        if os.path.exists(self._settings_file):
            os.remove(self._settings_file)
