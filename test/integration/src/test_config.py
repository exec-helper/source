import pytest
from pytest_bdd import scenarios, given, when, then

from scenarios_run import *

scenarios(
    "../feature/config",
    example_converters=dict(command=str, environment=Environment, pattern=PatternType),
)


@given(
    "the <environment> is configured for <command> command in the configuration",
    target_fixture="add_environment_to_command",
)
def add_environment_to_command(config, command, environment):
    config.commands[command].set_environment(environment)


@given(
    "the <pattern> is configured for <command> command in the configuration",
    target_fixture="add_pattern_to_command",
)
def add_pattern_to_command(config, command, pattern):
    config.commands[command].add_pattern(pattern)


@then("the runtime environment for <command> should contain the given <environment>")
def check_environment(run_environment, command, environment):
    runs = run_environment.config.commands[command].runs
    actual_env = runs[-1].environment

    assert len(runs) > 0

    for key, value in environment.items():
        for permutation in run_environment.config.commands[command].pattern_generator():
            replaced_key = key
            replaced_value = value
            for pattern_key, pattern_value in permutation.items():
                replaced_key = replaced_key.replace("{" + pattern_key + "}", pattern_value)
                replaced_value = replaced_value.replace("{" + pattern_key + "}", pattern_value)

            assert replaced_key in actual_env
            assert replaced_value == actual_env[replaced_key]
