#include <tuple>

#include <catch.hpp>

#include "core/options.h"
#include "core/task.h"
#include "core/pattern.h"
#include "plugins/selector.h"
#include "plugins/memory.h"

#include "optionsStub.h"
#include "executorStub.h"

#include "utils/utils.h"

using std::vector;
using std::string;
using std::get;

using execHelper::core::Task;
using execHelper::core::Pattern;

using execHelper::test::OptionsStub;
using execHelper::core::test::ExecutorStub;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::appendVectors;

namespace {
    void setupBasicOptions(OptionsStub& options, const Patterns& patterns) {
        addSettings(options.m_settings, "commands", {"selector-command"});
        addSettings(options.m_settings, "selector-command", {"selector"});
        addSettings(options.m_settings, "selector", {"selector-command"});

        for(const auto& pattern : patterns) {
            options.m_patternsHandler->addPattern(pattern);
        }
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default selector settings", "[plugins][selector]") {
        GIVEN("A selector plugin object and the default options") {
            OptionsStub options;

            Selector plugin;
            Task task;

            WHEN("We apply the selector plugin") {
                bool success = plugin.apply("random-command", task, options);

                THEN("It should not succeed") {
                    REQUIRE(success == false);
                }
            }
        }
    }

    SCENARIO("Test the selector plugin when no related arguments are passed on the command line", "[plugins][selector]") {
        GIVEN("A selector object and the correct configuration") {
            vector<string> selectionOptions1 = {"memory", "memory", "memory"};
            vector<string> selectionOptions2 = {"memory", "memory", "memory"};
            Pattern testPattern1("TESTPATTERN1", selectionOptions1, 't', "--test-pattern1");
            Pattern testPattern2("TESTPATTERN2", selectionOptions2, 'u', "--test-pattern2");
            const string command1("selector-command");
            const string command2("selector-command2");

            OptionsStub options;

            setupBasicOptions(options, {testPattern1, testPattern2});
            addSettings(options.m_settings["selector"][command1], "pattern", testPattern1.getKey());
            addSettings(options.m_settings["selector"], "pattern", testPattern2.getKey());

            Selector plugin;
            Task task;

            WHEN("We apply the selector to command 1") {
                bool returnCode = plugin.apply(command1, task, options);

                THEN("It should return that it succeeded") {
                    REQUIRE(returnCode == true);
                }
                THEN("All default actions should be executed") {
                    const Memory::Memories memories = Memory::getExecutions();
                    REQUIRE(memories.size() == selectionOptions1.size());
                    for(size_t i = 0; i < selectionOptions1.size(); ++i) {
                        REQUIRE(memories[i].command == command1);
                    }
                }
                THEN("They should be called with the appropriate values") {
                    const Memory::Memories memories = Memory::getExecutions();
                    for(size_t i = 0; i < memories.size(); ++i) {
                        for(size_t j = 0; j < i; ++j) {
                            // Note: tasks should point to different objects, but since
                            // the previous task is destroyed before the next one is constructed,
                            // it may happen that they point to the same chunk of memory.
                            // So this can not be checked
                            REQUIRE(memories[i].command == memories[j].command);
                            REQUIRE(memories[i].task == memories[j].task);
                            REQUIRE(memories[i].options == memories[j].options);
                        }
                    }
                }
                Memory::reset();
            }
            WHEN("We apply the selector to command 2") {
                bool returnCode = plugin.apply(command2, task, options);

                THEN("It should return that it succeeded") {
                    REQUIRE(returnCode == true);
                }
                THEN("All default actions should be executed") {
                    const Memory::Memories memories = Memory::getExecutions();
                    REQUIRE(memories.size() == selectionOptions2.size());
                    for(size_t i = 0; i < selectionOptions2.size(); ++i) {
                        REQUIRE(memories[i].command == command2);
                    }
                }
                THEN("They should be called with the appropriate values") {
                    const Memory::Memories memories = Memory::getExecutions();
                    for(size_t i = 0; i < memories.size(); ++i) {
                        for(size_t j = 0; j < i; ++j) {
                            // Note: tasks should point to different objects, but since
                            // the previous task is destroyed before the next one is constructed,
                            // it may happen that they point to the same chunk of memory.
                            // So this can not be checked
                            REQUIRE(memories[i].command == memories[j].command);
                            REQUIRE(memories[i].task == memories[j].task);
                            REQUIRE(memories[i].options == memories[j].options);
                        }
                    }
                }
                Memory::reset();
            }
        }
    }

    SCENARIO("Test the selector plugin when related arguments are passed on the command line", "[plugins][selector]") {
        GIVEN("A selector object and a the correct configuration") {
            const string command1("memory");
            const string command2("memory");
            const string commandlineCommand1("selector-command");
            const string commandlineCommand2("selector-command2");
            vector<string> selectionOptions1 = {command1, "command1", "command2"};
            vector<string> selectionOptions2 = {"command1", "command2", "command3"};
            vector<string> commandLineOptions1 = {command1};
            vector<string> commandLineOptions2 = {command2};
            Pattern testPattern1("TESTPATTERN1", selectionOptions1, 't', "--test-pattern1");
            Pattern testPattern2("TESTPATTERN2", selectionOptions2, 'u', "--test-pattern2");

            OptionsStub options;

            setupBasicOptions(options, {testPattern1, testPattern2});
            addSettings(options.m_settings["selector"][commandlineCommand1], "pattern", testPattern1.getKey());
            addSettings(options.m_settings["selector"], "pattern", testPattern2.getKey());

            options.m_options.insert(make_pair(testPattern1.getLongOption(), commandLineOptions1));
            options.m_options.insert(make_pair(testPattern2.getLongOption(), commandLineOptions2));

            Selector plugin;
            Task task;

            WHEN("We apply the selector with command 1") {
                REQUIRE(plugin.apply(commandlineCommand1, task, options) == true);

                THEN("All actions defined on the command line should be executed") {
                    ExecutorStub::TaskQueue executedTasks = options.m_executor.getExecutedTasks();
                    const Memory::Memories memories = Memory::getExecutions();
                    REQUIRE(memories.size() == commandLineOptions1.size());
                    for(size_t i = 0; i < commandLineOptions1.size(); ++i) {
                        REQUIRE(memories[i].command == commandlineCommand1);
                    }
                }
                THEN("They should be called with the appropriate values") {
                    const Memory::Memories memories = Memory::getExecutions();
                    for(size_t i = 0; i < memories.size(); ++i) {
                        for(size_t j = 0; j < i; ++j) {
                            // Note: tasks should point to different objects, but since
                            // the previous task is destroyed before the next one is constructed,
                            // it may happen that they point to the same chunk of memory.
                            // So this can not be checked
                            REQUIRE(memories[i].command == memories[j].command);
                            REQUIRE(memories[i].task == memories[j].task);
                            REQUIRE(memories[i].options == memories[j].options);
                        }
                    }
                }
                Memory::reset();
            }
            WHEN("We apply the selector with command 2") {
                REQUIRE(plugin.apply(commandlineCommand2, task, options) == true);

                THEN("All actions defined on the command line should be executed") {
                    ExecutorStub::TaskQueue executedTasks = options.m_executor.getExecutedTasks();
                    const Memory::Memories memories = Memory::getExecutions();
                    REQUIRE(memories.size() == commandLineOptions2.size());
                    for(size_t i = 0; i < commandLineOptions2.size(); ++i) {
                        REQUIRE(memories[i].command == commandlineCommand2);
                    }
                }
                THEN("They should be called with the appropriate values") {
                    const Memory::Memories memories = Memory::getExecutions();
                    for(size_t i = 0; i < memories.size(); ++i) {
                        for(size_t j = 0; j < i; ++j) {
                            // Note: tasks should point to different objects, but since
                            // the previous task is destroyed before the next one is constructed,
                            // it may happen that they point to the same chunk of memory.
                            // So this can not be checked
                            REQUIRE(memories[i].command == memories[j].command);
                            REQUIRE(memories[i].task == memories[j].task);
                            REQUIRE(memories[i].options == memories[j].options);
                        }
                    }
                }
                Memory::reset();
            }
        }
    }
    SCENARIO("Test the selector plugin when no pattern parameter is defined in the configuration", "[plugins][selector]") {
        GIVEN("A selector object and the correct configuration") {
            vector<string> selectionOptions = {"memory", "memory", "memory"};
            Pattern testPattern("TESTPATTERN", selectionOptions, 't', "--test-pattern");
            const string command("selector-command");

            OptionsStub options;

            setupBasicOptions(options, {testPattern});

            Selector plugin;
            Task task;

            WHEN("We apply the selector to command 1") {
                bool returnCode = plugin.apply(command, task, options);

                THEN("It should return that it succeeded") {
                    REQUIRE(returnCode == false);
                }
                THEN("All default actions should be executed") {
                    const Memory::Memories memories = Memory::getExecutions();
                    REQUIRE(memories.size() == 0U);
                }
                Memory::reset();
            }
        }
    }
} } }
