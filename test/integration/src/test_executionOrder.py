import pytest    
from pytest_bdd import scenarios, given, when, then, parsers

from scenarios_run import *

scenarios('../feature/execution-order/execution-order.feature', example_converters=dict(command = str, nb_of_times = int))

@when('run the <command> command <nb_of_times> in the same statement')
def run_command_n_times(run_environment, command, nb_of_times):
    args = [command for i in range(0, nb_of_times)]
    run_environment.run_application(args)

@then('the <command> command should be called <nb_of_times> times')
def command_called_times(run_environment, command, nb_of_times):
    assert(len(run_environment.config.commands[command].runs) == nb_of_times)
