import os
from pathlib import Path

import pytest
from pytest_bdd import scenarios, given, when, then

from scenarios_run import *

scenarios(
    "../feature/working-dir",
    example_converters=dict(command=str, start_working_dir=Path, expected_working_dir=Path),
)


@given(
    "a current working directory of <start_working_dir>",
    target_fixture="working_dir",
)
def working_dir(run_environment, start_working_dir):
    path = Path(start_working_dir)
    path.mkdir(parents=True, exist_ok=True)
    run_environment.set_working_dir(path)
    run_environment.config_is_external()
    return path


@then("the working directory should be the environment root dir")
def check_working_dir(run_environment, command):
    expected_working_dir = run_environment.root_dir
    last_run = run_environment.config.commands[command].runs[-1]

    assert last_run.working_dir == expected_working_dir


@then("the PWD environment variable should be the environment root dir")
def check_pwd(run_environment, command):
    expected_working_dir = run_environment.root_dir
    last_run = run_environment.config.commands[command].runs[-1]

    key = "PWD"

    assert key in last_run.environment
    assert Path(last_run.environment[key]) == expected_working_dir
