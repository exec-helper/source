from typing import List

import pytest
from pytest_bdd import scenarios, given, when, then

from scenarios_run import *

def ListOfStrings(string):
    return string.split(" ")

scenarios('../feature/cmd-args', example_converters=dict(command_line = ListOfStrings, command = str, pattern = PatternType))

@when('we call the application with the <command_line> options')
def call_no_command(run_environment, command_line):
    run_environment.run_application(command_line)

@when('we call the application without arguments')
def call_no_option(run_environment):
    run_environment.run_application([])
