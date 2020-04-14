import tempfile
from pathlib import Path

import pytest
from pytest_bdd import scenarios, given, when, then

from custom_plugin import CustomPlugin
from command import Command

from scenarios_run import *

scenarios('../feature/custom-plugins', example_converters=dict(command_line = CommandLineArgs, description = str, plugin_id = str, command = str, nb_of_times = int))

def register_command_for_plugin(config, command, plugin_id):
    custom_command = Command(command, plugin_id)
    config.add_command(command, custom_command)

@given('a random custom plugin directory')
def custom_plugin_dir():
    return tempfile.gettempdir()

@given('a custom module with id <plugin_id>')
def custom_plugin(plugin_id, custom_plugin_dir):
    return CustomPlugin(plugin_id, custom_plugin_dir)

@given('the same custom module <plugin_id> on a different location and add it to the command line search path')
def other_custom_plugin(plugin_id, run_environment, custom_plugin_dir):
    other_location = custom_plugin_dir + '/blaat'
    Path(other_location).mkdir(parents=True, exist_ok=True)
    add_search_path_commandline(run_environment, other_location)
    return CustomPlugin(plugin_id, other_location)

@given('a registered command <command> that uses the module <plugin_id>')
def registered_command_custom_plugin(config, command, plugin_id):
    register_command_for_plugin(config, command, plugin_id)

@given('the custom plugin search path is registered in the configuration')
def search_path_registered(config, custom_plugin_dir):
    config.add_plugin_search_path(custom_plugin_dir)

@when('we register the command <command> to use the module <plugin_id>')
def register_command_plugin(config, command, plugin_id):
    register_command_for_plugin(config, command, plugin_id)

@when('add the search path to the configuration')
def add_search_path_configuration(config, custom_plugin_dir):
    config.add_plugin_search_path(custom_plugin_dir)

@when('add the search path to the command line')
def add_search_path_commandline(run_environment, custom_plugin_dir):
    run_environment.add_commandline(['--additional-search-path', custom_plugin_dir])

@then('stdout should contain <plugin_id>')
def stdout_plugin_id(run_environment, plugin_id):
    stdout_contains(run_environment, plugin_id)

@then('stdout should contain regex <description>')
def stdout_plugin_id(run_environment, description):
    stdout_contains_regex(run_environment, description)
