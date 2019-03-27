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
        self._args['--settings-file'] = self._config.file

    def write_config(self):
        self._config.write()

    def create_command(self, command):
        """ Create a command in the given config """
        self._config.create_command(command)

    def set_environment(self, command, envs):
        """ Set the environment for the given command """
        envs = TestEnvironment.list2dict(envs)
        self._config.set_environment(command, envs)

    def run_application(self, arg_list=[]):
        args = ['exec-helper']

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

    @staticmethod
    def list2dict(input):
        result = dict()
        for value in input:
            parts = value.split(":")
            if len(parts) != 2:
                raise AssertionError("Cannot parse '{value}' to dict".format(value = value))
            result[parts[0]] = parts[1]
        return result

    def environment_contains(self, command, envs):
        envs = TestEnvironment.list2dict(envs)

        runs = self._config.commands[command].runs

        if not runs:
           raise AssertionError("Command '{command}' was not executed".format(command = command)) 

        for key,value in envs.items():
            for run in runs:
                if key not in run.environment:
                    raise AssertionError("{key} is expected to occur in the environment".format(key = key))

                if not value == run.environment[key]:
                    raise AssertionError("Actual value '{value}' is not the expected value '{expected}'".format(actual = value, expected = value))
