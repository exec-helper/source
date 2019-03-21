import subprocess

import WorkingDir
import Config

class TestEnvironment(object):
    def __init__(self):
        self._working_dir = WorkingDir.WorkingDir()
        self._config = Config.Config(self._working_dir.dir)
        self._args = {}

    def no_config(self):
        self._config.remove()

    def add_valid_config(self):
        self._config.write()
        self._args['--settings-file'] = self._config.file

    def run_application(self, arg_list=[]):
        args = ['exec-helper']

        print(len(arg_list))

        for cmd_option,value in self._args.items():
            args.extend([cmd_option, value])
        args.extend(arg_list)
        print(args)
        self._last_run = subprocess.run(args, cwd = self._working_dir.dir, capture_output = True, check = False) 

    def stdout_contains(self, needle):
        if not str.encode(needle) in self._last_run.stdout:
            raise AssertionError("'{needle}' not found in stdout: '{stdout}'".format(needle = needle, stdout = self._last_run.stdout))

    def stderr_contains(self, needle):
        if not str.encode(needle) in self._last_run.stderr:
            raise AssertionError("'{needle}' not found in stderr: '{stderr}'".format(needle = needle, stderr = self._last_run.stderr))

    def print_stdout(self):
        print(self._last_run.stdout)

    def print_stderr(self):
        print(self._last_run.stderr)

    def check_return_code(self, expected):
        expected = int(expected)
        if self._last_run.returncode != expected:
            raise AssertionError("Return code was '{actual}' but expected '{expected}'".format(actual = self._last_run.returncode, expected = expected))
