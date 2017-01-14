#include <tuple>

#include <catch.hpp>

#include "core/options.h"
#include "core/task.h"
#include "core/pattern.h"
#include "plugins/executePlugin.h"
#include "plugins/commandLineCommand.h"
#include "plugins/scons.h"
#include "plugins/make.h"
#include "plugins/bootstrap.h"
#include "plugins/cppcheck.h"
#include "plugins/clangStaticAnalyzer.h"
#include "plugins/selector.h"
#include "plugins/memory.h"

#include "optionsStub.h"
#include "executorStub.h"

#include "utils/utils.h"

using std::vector;
using std::string;
using std::unique_ptr;

using execHelper::core::Task;
using execHelper::core::Pattern;

using execHelper::plugins::Plugin;
using execHelper::plugins::CommandLineCommand;
using execHelper::plugins::Scons;
using execHelper::plugins::Make;
using execHelper::plugins::Bootstrap;
using execHelper::plugins::Cppcheck;
using execHelper::plugins::ClangStaticAnalyzer;
using execHelper::plugins::Selector;
using execHelper::plugins::Memory;
using execHelper::plugins::ExecutePlugin;

using execHelper::test::OptionsStub;
using execHelper::core::test::ExecutorStub;
using execHelper::test::utils::Patterns;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::appendVectors;

namespace {
    void setupBasicOptions(OptionsStub& options, const Patterns& patterns) {
        addSettings(options.m_settings, "commands", {"execute-command"});
        addSettings(options.m_settings, "execute-command", {"execute-plugin"});
        addSettings(options.m_settings, "execute-plugin", {"execute-command"});

        for(const auto& pattern : patterns) {
            options.m_patternsHandler->addPattern(pattern);
        }
    }

    template<typename T>
    bool checkGetPlugin(const string& pluginName) {
        unique_ptr<Plugin> plugin = ExecutePlugin::getPlugin(pluginName);
        T* derived = dynamic_cast<T*>(plugin.get());  // derived will be a nullptr if the cast fails
        return (derived > static_cast<T*>(0));
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default execute settings", "[plugins][execute]") {
        GIVEN("A selector plugin object and the default options") {
            OptionsStub options;

            ExecutePlugin plugin({});
            Task task;

            WHEN("We apply the selector plugin") {
                bool success = plugin.apply("random-command", task, options);

                THEN("It should succeed") {
                    REQUIRE(success == true);
                }
            }
        }
    }

    SCENARIO("Test the execute plugin when no related arguments are passed on the command line", "[plugins][execute]") {
        GIVEN("A execute plugin object and the correct configuration") {
            vector<string> selectionOptions = {"memory", "memory", "memory"};
            const string command("execute-command");

            OptionsStub options;

            setupBasicOptions(options, {});

            ExecutePlugin plugin(selectionOptions);
            Task task;

            WHEN("We apply the executor") {
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
    
    SCENARIO("Test problematic cases", "[plugins][execute]") {
        GIVEN("A plugin with an non-existing plugin to execute") {
            ExecutePlugin plugin({"non-existing-plugin"});

            Task task;
            OptionsStub options;

            WHEN("We execute the plugin") {
                bool returnCode = plugin.apply("test", task, options);

                THEN("It should not succeed") {
                    REQUIRE(returnCode == false);
                }
            }
        }
        GIVEN("A plugin that fails to execute") {
            Memory::setReturnCode(false);

            ExecutePlugin plugin({"memory", "memory"});

            Task task;
            OptionsStub options;

            WHEN("We execute the plugin") {
                bool returnCode = plugin.apply("test", task, options);

                THEN("It should not succeed") {
                    REQUIRE(returnCode == false);
                }
                THEN("It should have stopped executing after the failure") {
                    REQUIRE(Memory::getExecutions().size() == 1U);
                }
                Memory::reset();
            }
        }
    }

    // Note: this test required RTTI support
    SCENARIO("Testing the plugin getter", "[plugins][execute]") {
        GIVEN("Nothing in particular") {
            WHEN("We request the respective plugin object") {
                THEN("We should get the appropriate ones") {
                   REQUIRE(checkGetPlugin<CommandLineCommand>("command-line-command")); 
                   REQUIRE(checkGetPlugin<Scons>("scons")); 
                   REQUIRE(checkGetPlugin<Make>("make")); 
                   REQUIRE(checkGetPlugin<Bootstrap>("bootstrap")); 
                   REQUIRE(checkGetPlugin<Cppcheck>("cppcheck")); 
                   REQUIRE(checkGetPlugin<ClangStaticAnalyzer>("clang-static-analyzer")); 
                   REQUIRE(checkGetPlugin<Selector>("selector")); 
                   REQUIRE(checkGetPlugin<Memory>("memory")); 
                }
            }
            WHEN("We try to get a non-existing plugin") {
                THEN("We should not get anything") {
                    REQUIRE_FALSE(ExecutePlugin::getPlugin("non-existing-plugin").get());
                }
            }
        }
    }
} } }
