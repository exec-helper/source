import subprocess

import WorkingDir
import Config
from pattern import Pattern

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

    def add_pattern(self, command, pattern_string):
        """ Add the list of patterns to the given command """
        parts = pattern_string.split(":")
        if len(parts) != 2:
            raise AssertionError("Cannot parse '{value}' to pattern".format(value = value))
        pattern_id = parts[0]
        pattern_values = parts[1].split(',')
        pattern = Pattern(pattern_id, pattern_values)
        self._config.add_pattern(command, pattern)

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

    @staticmethod
    def permutate_patterns(patterns):
        if not patterns:
            yield dict()
            return

        pattern = patterns[0]
        if not pattern.values:
            for permutation in TestEnvironment.permutate_patterns(patterns[1:]):
                yield permutation

        for value in pattern.values:
            for permutation in TestEnvironment.permutate_patterns(patterns[1:]):
                permutation[pattern.id] = value
                yield permutation

    def pattern_generator(self, command):
        patterns = self._config.commands[command].patterns
        
        for permutation in TestEnvironment.permutate_patterns(patterns):
            yield permutation

    def environment_contains(self, command, envs):
        envs = TestEnvironment.list2dict(envs)

        runs = self._config.commands[command].runs

        if not runs:
           raise AssertionError("Command '{command}' was not executed".format(command = command)) 

        # Replace patterns and require it to occur in at least one of the runs
        for env_key, env_value in envs.items():
            for pattern_values in self.pattern_generator(command):
                key = env_key
                value = env_value

                for pattern_key,pattern_value in pattern_values.items():
                    pattern_signature = "{" + pattern_key + "}"
                    key = key.replace(pattern_signature, pattern_value)
                    value = value.replace(pattern_signature, pattern_value)

                environment_found = False
                for run in runs:
                    if key not in run.environment:
                        print("INFO: Key '{key}' not found in the environment of this run".format(key = key))
                        continue

                    if value == run.environment[key]:
                        environment_found = True
                        break
                    else:
                        print("INFO: Value '{actual}' does not match expected value '{expected}' for key '{key}'".format(key = key, actual = run.environment[key], expected = value))
                        print("Continuing search")

                if not environment_found:
                    raise AssertionError("Expected environment value '{expected}' was not found in any environment".format(expected = value))
