import json
from pathlib import Path
import re
import sys
import tempfile
import uuid

import pytest
from pytest_bdd import scenarios, given, when, then, parsers

from run_environment import RunEnvironment
from config import Config
from pattern import Pattern

# Conversion types
def Environment(string):
    pairs = string.split(";")
    return { pair.split(":")[0]: pair.split(":")[1] for pair in pairs }

def CommandLineArgs(string):
    return string.split(" ")

def PatternType(string):
    characteristics = json.loads(string)
    key = characteristics['key']
    default_values = characteristics['default_values']

    long_options = []
    if 'long_options' in characteristics:
        long_options = characteristics['long_options']
    return Pattern(key, default_values, long_options)

@given('a controlled environment')
def run_environment():
    temp_directory = tempfile.gettempdir()
    temp_folder = 'eh-' + str(uuid.uuid4())
    root_dir = Path(f"{temp_directory}/{temp_folder}")
    return RunEnvironment(root_dir)

@given('a valid configuration')
def config(run_environment):
    run_environment.config = Config(run_environment.root_dir)
    return run_environment.config

@given('the <command> command')
def a_command(config, command):
    config.create_command(command)
    return config.commands[command]

@given('the <pattern> pattern')
def a_pattern(config, pattern):
    config.add_pattern(pattern)
    return pattern

@when('we add the <command> command')
def add_command(config, command):
    config.create_command(command)

@when('we run the <command> command')
def run_one_command(run_environment, command):
    run_environment.run_application([command])

@when('we call the application with the <command_line> options')
def call_no_command(run_environment, command_line):
    run_environment.run_application(command_line)

@when('we call the application without arguments')
def call_no_option(run_environment):
    run_environment.run_application([])

@then('the call should succeed')
def call_succeeds(run_environment):
    if run_environment.last_run.returncode != 0:
        print(run_environment.last_run.stdout)
        print(run_environment.last_run.stderr, file = sys.stderr)
        raise AssertionError(f"Call was expected to succeed, but it failed with return code '{run_environment.last_run.returncode}'")

@then(parsers.cfparse('the call should fail with return code {expected:Number}', extra_types=dict(Number=int)))
def call_fails(run_environment, expected):
    if run_environment.last_run.returncode != expected:
        print(run_environment.last_run.stdout)
        print(run_environment.last_run.stderr, file = sys.stderr)
        raise AssertionError(f"Call was expected to fail with return code '{expected}', but it succeeded")

@then(parsers.parse('stdout should contain {expected}'))
def stdout_contains(run_environment, expected):
    expected = expected.strip("'")
    if not expected.encode('utf-8') in run_environment.last_run.stdout:
        print(run_environment.last_run.stdout, file = sys.stdout)
        raise AssertionError(f"Stdout was expected to contain '{expected}', but it did not")

@then(parsers.parse('stdout should contain regex {expected}'))
def stdout_contains_regex(run_environment, expected):
    expected = expected.strip("'")
    regex = re.compile(expected.encode('utf-8'))
    if not regex.search(run_environment.last_run.stdout):
        print(run_environment.last_run.stdout, file = sys.stdout)
        raise AssertionError(f"Regex {expected} did not match stdout")

@then(parsers.parse('stdout should not contain {expected}'))
def stdout_not_contains(run_environment, expected):
    expected = expected.strip("'")
    if expected.encode('utf-8') in run_environment.last_run.stdout:
        print(run_environment.last_run.stdout, file = sys.stdout)
        raise AssertionError(f"Stdout was expected to _not_ contain '{expected}', but it did")

@then(parsers.parse('stderr should contain {expected}'))
def stderr_contains(run_environment, expected):
    expected = expected.strip("'")
    if not expected.encode('utf-8') in run_environment.last_run.stderr:
        print(run_environment.last_run.stderr, file = sys.stderr)
        raise AssertionError(f"Stderr was expected to contain '{expected}', but it did not")

@then(parsers.parse('stderr should not contain {expected}'))
def stderr_not_contains(run_environment, expected):
    expected = expected.strip("'")
    if expected.encode('utf-8') in run_environment.last_run.stderr:
        print(run_environment.last_run.stderr, file = sys.stderr)
        raise AssertionError(f"Stderr was expected to _not_ contain '{expected}', but it did")

@then('stderr should be empty')
def stderr_is_empty(run_environment):
    stderr = run_environment.last_run.stderr
    if stderr:
        raise AssertionError(f"Stderr is expected to be empty, but contains: '{stderr}'")

@when('run the <command> command <nb_of_times> in the same statement')
def run_command_n_times(run_environment, command, nb_of_times):
    args = [command for i in range(0, nb_of_times)]
    run_environment.run_application(args)

@then('the <command> command should be called <nb_of_times> times')
def command_called_times(run_environment, command, nb_of_times):
    assert(len(run_environment.config.commands[command].runs) == nb_of_times)
