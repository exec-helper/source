import sys
import json

import pytest
from pytest_bdd import scenarios, given, when, then, parsers

from run_environment import RunEnvironment
from config import Config
from pattern import Pattern

# Conversion types
def Environment(string):
    pairs = string.split(";")
    return { pair.split(":")[0]: pair.split(":")[1] for pair in pairs }

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
    return RunEnvironment()

@given('a valid configuration')
def config(run_environment):
    run_environment.add_valid_config()
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

@then('the call should succeed')
def call_succeeds(run_environment):
    if run_environment.last_run.returncode != 0:
        print(run_environment.last_run.stdout)
        print(run_environment.last_run.stderr, file = sys.stderr)
        assert(False)
    assert(True)

@then(parsers.cfparse('the call should fail with return code {expected:Number}', extra_types=dict(Number=int)))
def call_fails(run_environment, expected):
    if run_environment.last_run.returncode != expected:
        print(run_environment.last_run.stdout)
        print(run_environment.last_run.stderr, file = sys.stderr)
        assert(False)
    assert(True)

@then('stdout should contain <command>')
@then(parsers.parse('stdout should contain {expected}'))
def stdout_contains(run_environment, expected):
    expected = expected.strip("'")
    if not expected.encode('utf-8') in run_environment.last_run.stdout:
        print(run_environment.last_run.stdout, file = sys.stderr)
        assert(False)
    assert(True)

@then(parsers.parse('stdout should not contain {expected}'))
def stdout_contains(run_environment, expected):
    expected = expected.strip("'")
    if expected.encode('utf-8') in run_environment.last_run.stdout:
        print(run_environment.last_run.stdout, file = sys.stderr)
        assert(False)
    assert(True)

@then(parsers.parse('stderr should contain {expected}'))
def stderr_contains(run_environment, expected):
    expected = expected.strip("'")
    if not expected.encode('utf-8') in run_environment.last_run.stderr:
        print(run_environment.last_run.stdout, file = sys.stderr)
        assert(False)
    assert(True)

@then('stderr should be empty')
def stderr_is_empty(run_environment):
    stderr = run_environment.last_run.stderr
    if stderr:
        print(f"Stderr is expected to be empty, but contains: '{stderr}'")
        assert(False)
    assert(True)

