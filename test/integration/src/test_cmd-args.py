from typing import List

import pytest
from pytest_bdd import scenarios, given, when, then

from scenarios_run import *

scenarios('../feature/cmd-args', example_converters=dict(command_line = CommandLineArgs, command = str, pattern = PatternType))

@then('stdout should contain <command>')
def stdout_plugin_id(run_environment, command):
    stdout_contains(run_environment, command)
