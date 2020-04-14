import subprocess

from working_dir import WorkingDir
from config import Config
from pattern import Pattern

class RunEnvironment(object):
    def __init__(self):
        self._working_dir = WorkingDir()
        self._config = Config(self._working_dir.dir)
        self._args = ['exec-helper']

    @property
    def config(self):
        return self._config

    @property
    def last_run(self):
        return self._last_run

    def no_config(self):
        self._config.remove()

    def add_valid_config(self):
        self._args.append('--settings-file')
        self._args.append(self._config.file)

    def add_pattern(self, command, pattern_string):
        """ Add the list of patterns to the given command """
        parts = pattern_string.split(":")
        if len(parts) != 2:
            raise AssertionError("Cannot parse '{value}' to pattern".format(value = value))
        pattern_id = parts[0]
        pattern_values = parts[1].split(',')
        pattern = Pattern(pattern_id, pattern_values)
        self._config.add_pattern(command, pattern)

    def add_commandline(self, arg_list):
        self._args.extend(arg_list)

    def run_application(self, arg_list=[]):
        if self._config:
            self._config.write()

        args = self._args
        args.extend(arg_list)
        print(' '.join(args))
        self._last_run = subprocess.run(args, cwd = self._working_dir.dir, capture_output = True, check = False) 

