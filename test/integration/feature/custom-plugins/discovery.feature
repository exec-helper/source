@cmd_args @custom_plugins @custom_plugins_discovery
Feature: Discover custom plugins
    Scenarios for discovering custom plugins at runtime

    Examples:
    | command_line   |
    | --list-plugins |

    Background:
        Given a controlled environment
        And a valid configuration
        And a random custom plugin directory

    @successful
    Scenario: Discover the system modules
        When we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain <plugin_id>
        And stdout should contain regex <description>

        Examples:
        | plugin_id             | description                                       |
        | bootstrap             | Lua plugin for module \S*bootstrap.lua            |
        | clang-static-analyzer | Lua plugin for module \S*clang-static-analyzer.lua|
        | clang-tidy            | Lua plugin for module \S*clang-tidy.lua           |
        | command-line-command  | Command-line-command \(internal\)                 |
        | commands              | Command \(internal\)                              |
        | cppcheck              | Lua plugin for module \S*cppcheck.lua             |
        | lcov                  | Lcov \(internal\)                                 |
        | memory                | Memory \(internal\)                               |
        | make                  | Lua plugin for module \S*make.lua                 |
        | ninja                 | Lua plugin for module \S*ninja.lua                |
        | pmd                   | Pmd \(internal\)                                  |
        | scons                 | Lua plugin for module \S*scons.lua                |
        | selector              | Lua plugin for module \S*selector.lua             |
        | valgrind              | Valgrind \(internal\)                             |

    @error
    Scenario: Fail to find a custom module when the search path is not set properly
        Given a custom module with id <plugin_id>
        When we register the command <command> to use the module <plugin_id>
        And we call the application with the <command_line> options
        Then the call should succeed
        And stdout should not contain <plugin_id>

        Examples:
        | plugin_id                 | command  |
        | exec-helper-custom-module | Command1 |

    @successful
    Scenario: Discover a custom module by setting the search path in the configuration
        Given a custom module with id <plugin_id>
        When we register the command <command> to use the module <plugin_id>
        And add the search path to the configuration
        And we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain <plugin_id>
        And stdout should contain regex <description>

        Examples:
        | plugin_id                 | description                                               | command  |
        | exec-helper-custom-module | Lua plugin for module /tmp/exec-helper-custom-module.lua  | Command1 |

    @successful
    Scenario: The search custom plugin configuration takes precedence over the sytem modules
        Given a custom module with id <plugin_id>
        When we register the command <command> to use the module <plugin_id>
        And add the search path to the configuration
        And we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain <plugin_id>
        And stdout should contain regex <description>

        Examples:
        | plugin_id             | description                                         | command    |
        | make                  | Lua plugin for module /tmp/make.lua                 | Command1   |
        | command-line-command  | Lua plugin for module /tmp/command-line-command.lua | Command1   |

    @successful
    Scenario: Discover a custom module by setting the search path on the command line
        Given a custom module with id <plugin_id>
        When we register the command <command> to use the module <plugin_id>
        And add the search path to the command line
        And we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain <plugin_id>
        And stdout should contain regex <description>

        Examples:
        | plugin_id                 | description                                               | command  |
        | exec-helper-custom-module | Lua plugin for module /tmp/exec-helper-custom-module.lua  | Command1 |

    @successful
    Scenario: The search stuom plugin command line takes precedence over the system modules
        Given a custom module with id <plugin_id>
        When we register the command <command> to use the module <plugin_id>
        And add the search path to the command line
        And we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain <plugin_id>
        And stdout should contain regex <description>

        Examples:
        | plugin_id             | description                                         | command    |
        | make                  | Lua plugin for module /tmp/make.lua                 | Command1   |
        | command-line-command  | Lua plugin for module /tmp/command-line-command.lua | Command1   |

    @successful
    Scenario: The search custom plugin command line parameter takes precedence over the one(s) in the configuration
        Given a custom module with id <plugin_id>
        And the same custom module <plugin_id> on a different location and add it to the command line search path
        When we register the command <command> to use the module <plugin_id>
        And add the search path to the configuration
        And we call the application with the <command_line> options
        Then the call should succeed
        And stdout should contain <plugin_id>
        And stdout should contain regex <description>

        Examples:
        | plugin_id             | description                                         | command    |
        | exec-helper-custom-module | Lua plugin for module /tmp/blaat/exec-helper-custom-module.lua  | Command1 |
