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
            vector<string> selectionOptions = {"memory", "memory", "memory"};
            Pattern testPattern("TESTPATTERN", selectionOptions, 't', "--test-pattern");
            const string command("selector-command");

            OptionsStub options;

            setupBasicOptions(options, {testPattern});
            addSettings(options.m_settings["selector"][command], "pattern", testPattern.getKey());

            Selector plugin;
            Task task;

            WHEN("We apply the selector") {
                bool returnCode = plugin.apply(command, task, options);

                THEN("It should return that it succeeded") {
                    REQUIRE(returnCode == true);
                }
                THEN("All default actions should be executed") {
                    const Memory::Memories memories = Memory::getExecutions();
                    REQUIRE(memories.size() == selectionOptions.size());
                    for(size_t i = 0; i < selectionOptions.size(); ++i) {
                        REQUIRE(memories[i].command == command);
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
            const string command("memory");
            const string commandlineCommand("selector-command");
            vector<string> selectionOptions = {command, "command1", "command2"};
            vector<string> commandLineOptions = {command};
            Pattern testPattern("TESTPATTERN", selectionOptions, 't', "--test-pattern");

            OptionsStub options;

            setupBasicOptions(options, {testPattern});
            addSettings(options.m_settings["selector"][commandlineCommand], "pattern", testPattern.getKey());

            options.m_options.insert(make_pair(testPattern.getLongOption(), commandLineOptions));

            Selector plugin;
            Task task;

            WHEN("We apply the selector") {
                REQUIRE(plugin.apply(commandlineCommand, task, options) == true);

                THEN("All actions defined on the command line should be executed") {
                    ExecutorStub::TaskQueue executedTasks = options.m_executor.getExecutedTasks();
                    const Memory::Memories memories = Memory::getExecutions();
                    REQUIRE(memories.size() == commandLineOptions.size());
                    for(size_t i = 0; i < commandLineOptions.size(); ++i) {
                        REQUIRE(memories[i].command == commandlineCommand);
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
} } }
