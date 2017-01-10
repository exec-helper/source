#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/bootstrap.h"

#include "core/execHelperOptions.h"

#include "utils/utils.h"

#include "executorStub.h"
#include "optionsStub.h"

using std::vector;
using std::string;
using std::ofstream;

using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CommandCollection;
using execHelper::core::Pattern;
using execHelper::core::PatternsHandler;

using execHelper::test::OptionsStub;
using execHelper::core::test::ExecutorStub;
using execHelper::test::utils::addSettings;
using execHelper::test::utils::CompilerUtil;

namespace {
    typedef vector<Pattern> Patterns;

    void setupBasicOptions(OptionsStub& options, const Patterns& patterns) {
        addSettings(options.m_settings, "commands", {"init"});
        addSettings(options.m_settings, "init", {"bootstrap"});

        for(const auto& pattern : patterns) {
            options.m_patternsHandler->addPattern(pattern);
        }
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default options of the bootstrap plugin", "[plugins][bootstrap]") {
        GIVEN("A bootstrap plugin object and basic settings") {
            CompilerUtil compilerUtil;
            OptionsStub options;
            setupBasicOptions(options, compilerUtil.getPatterns());

            Bootstrap plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("random-command", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"./bootstrap.sh"}));
                    expectedQueue.push_back(expectedTask);
                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the build-dir option of the bootstrap plugin", "[plugins][bootstrap]") {
        GIVEN("A bootstrap plugin object, basic settings and the build-dir option") {
            CompilerUtil compilerUtil;

            string command1("command1");
            string command2("command2");
            string filename("random-bootstrap");
            string buildDir("build/{COMPILER}/{MODE}/{HELLO}/{ARCHITECTURE}/hello{DISTRIBUTION}world");
            OptionsStub options;
            setupBasicOptions(options, compilerUtil.getPatterns());
            addSettings(options.m_settings["bootstrap"], "filename", filename);
            addSettings(options.m_settings["bootstrap"], command2, "build-dir");
            addSettings(options.m_settings["bootstrap"][command2], "build-dir", buildDir);
            addSettings(options.m_settings["bootstrap"][command2], "patterns", compilerUtil.getKeys());

            Bootstrap plugin;

            WHEN("We call the first command") {
                Task task;
                REQUIRE(plugin.apply(command1, task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    Task expectedTask;
                    expectedTask.append(TaskCollection({"./" + filename}));
                    expectedQueue.push_back(expectedTask);
                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
            WHEN("We call the second command") {
                Task task;
                REQUIRE(plugin.apply(command2, task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& pattern : compilerUtil.makePatternPermutator()) {
                        string compilerName = pattern.at(compilerUtil.compiler.getKey());
                        string modeName = pattern.at(compilerUtil.mode.getKey());
                        string architectureName = pattern.at(compilerUtil.architecture.getKey());
                        string distributionName = pattern.at(compilerUtil.distribution.getKey());

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"cd", "build/" + compilerName + "/" + modeName + "/{HELLO}/" + architectureName + "/hello" + distributionName + "world", "&&", "./" + filename}));
                        expectedQueue.push_back(expectedTask);
                    }
                    REQUIRE(expectedQueue == options.m_executor.getExecutedTasks());
                }
            }
        }
    }
} } }
