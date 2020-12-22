from pathlib import Path
import os
import yaml

import command


class Config(object):
    def __init__(self, directory, filename=".exec-helper"):
        self._settings_file = Path(directory).joinpath(filename)
        self._directory = directory
        self._commands = dict()
        self._patterns = set()
        self._plugin_search_path = []
        if os.path.exists(self._settings_file):
            raise AssertionError(
                "Temporary file '{file}' already exists!".format(file=self._settings_file)
            )

    def __del__(self):
        # self.remove()
        pass

    @property
    def directory(self):
        return self._directory

    @property
    def file(self):
        return self._settings_file

    @property
    def commands(self):
        return self._commands

    def create_command(self, command_id):
        """Creates a command for the given command id using an implementation-specific plugin"""
        self._commands[command_id] = command.Command(
            command_id, "command-line-command", self._directory
        )

    def add_command(self, command):
        """ Adds the given command as a command associated with the command id to the configuration """
        self._commands[command.id] = command

    def set_environment(self, cmd, envs):
        self._commands[cmd].set_environment(envs)

    def add_pattern(self, pattern):
        self._patterns.add(pattern)

    def add_plugin_search_path(self, path):
        self._plugin_search_path.append(str(path))

    def write(self):
        config_file = dict()

        # Make sure the config file is not empty
        config_file["blaat"] = []

        if self._plugin_search_path:
            config_file["additional-search-paths"] = self._plugin_search_path

        if self._patterns:
            config_file["patterns"] = {}
            for pattern in self._patterns:
                config_file["patterns"][pattern.id] = {"default-values": pattern.default_values}
                if pattern.long_options:
                    config_file["patterns"][pattern.id]["long-option"] = pattern.long_options

        if self._commands:
            config_file["commands"] = []

        for id, cmd in self._commands.items():
            config_file["commands"].append(id)

            config_file.update(cmd.to_dict())
            cmd.write_binary()

        with open(self._settings_file, "w") as f:
            yaml.dump(config_file, f)

    def remove(self):
        if os.path.exists(self._settings_file):
            os.remove(self._settings_file)
