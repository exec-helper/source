import subprocess

from config import Config
from pattern import Pattern

class RunEnvironment(object):
    def __init__(self, root_dir):
        self._config = None
        self._root_dir = root_dir
        self._working_dir = self._root_dir
        self._args = ['exec-helper']

        self._root_dir.mkdir(parents=True, exist_ok=False)
        print(f"Creating simulation environment in directory '{self._root_dir}'")

    def __del__(self):
        # self.remove()
        pass

    @property
    def config(self):
        return self._config

    @config.setter
    def config(self, value):
        self._config = value

    @property
    def last_run(self):
        return self._last_run

    @property
    def working_dir(self):
        return self._working_dir

    @property
    def root_dir(self):
        return self._root_dir

    def set_working_dir(self, newWorkingDir):
        self._working_dir = newWorkingDir

    def no_config(self):
        self._config.remove()

    def config_is_external(self):
        self._args.append('--settings-file')
        self._args.append(str(self._config.file))

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
        print("Executing '" + ' '.join(args) + "'")
        self._last_run = subprocess.run(args, cwd = self._working_dir, capture_output = True, check = False) 

    def remove(self):
        if os.path.is_directory(self._root_dir):
            shutil.rmtree(self._root_dir)
