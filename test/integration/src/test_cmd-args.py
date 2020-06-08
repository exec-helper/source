from typing import List

import pytest
from pytest_bdd import scenarios, given, when, then

from scenarios_run import *

scenarios('../feature/cmd-args', example_converters=dict(command_line = CommandLineArgs, command = str, pattern = PatternType, nb_of_times = int, return_code = int))

@then('stdout should contain <command>')
def stdout_plugin_id(run_environment, command):
    stdout_contains(run_environment, command)

@then('stdout should contain the full line <command>')
def stdout_plugin_regex_command(run_environment, command):
    stdout_contains_regex(run_environment, f'^{command}$')

@then('stdout should contain the full line <pattern>')
def stdout_plugin_regex_pattern(run_environment, pattern):
    for option in pattern.long_options:
        stdout_contains_regex(run_environment, f'^--{option}$')
