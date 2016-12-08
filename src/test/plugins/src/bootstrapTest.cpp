#include <vector>
#include <string>
#include <fstream>

#include <catch.hpp>

#include "plugins/bootstrap.h"

#include "executorStub.h"
#include "core/execHelperOptions.h"
#include "core/compilerDescription.h"
#include "core/targetDescription.h"

#include "utils/utils.h"

#include "optionsStub.h"

using std::vector;
using std::string;
using std::ofstream;

using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::CompilerDescription;
using execHelper::core::TargetDescription;
using execHelper::core::CommandCollection;

using execHelper::test::utils::addSettings;
using execHelper::test::OptionsStub;

namespace {
    void setupBasicOptions(OptionsStub& options, const TargetDescription& targets, const CompilerDescription& compilers) {
        addSettings(options.m_settings, "commands", {"init"});
        addSettings(options.m_settings, "init", {"bootstrap"});

        options.m_targets = targets;
        options.m_compilers = compilers;
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default options of the bootstrap plugin", "[plugins][bootstrap]") {
        GIVEN("A bootstrap plugin object and basic settings") {
            const TargetDescription actualTargets({"target1", "target2"}, {"runTarget1", "runTarget2"});
            const CompilerDescription actualCompilers(CompilerDescription::CompilerNames({"compiler1", "compiler2"}), 
                                                                                         {"mode1", "mode2"}, 
                                                                                         {"architectureA", "architectureB"},
                                                                                         {"distribution1", "distribution2"}
                                                                                        );

            OptionsStub options;
            setupBasicOptions(options, actualTargets, actualCompilers);
            ExecutorStub executor;
            options.setExecutor(&executor);

            Bootstrap plugin;

            WHEN("We use the plugin") {
                Task task;
                REQUIRE(plugin.apply("random-command", task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& compiler : actualCompilers) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"./bootstrap.sh"}));
                        expectedQueue.push_back(expectedTask);
                    }
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }

    SCENARIO("Testing the build-dir option of the bootstrap plugin", "[plugins][bootstrap]") {
        GIVEN("A bootstrap plugin object, basic settings and the build-dir option") {
            const TargetDescription actualTargets({"target1", "target2"}, {"runTarget1", "runTarget2"});
            const CompilerDescription actualCompilers(CompilerDescription::CompilerNames({"compiler1", "compiler2"}), 
                                                                                         {"mode1", "mode2"}, 
                                                                                         {"architectureA", "architectureB"},
                                                                                         {"distribution1", "distribution2"}
                                                                                        );

            string command1("command1");
            string command2("command2");
            string filename("random-bootstrap");
            string buildDir("build/{COMPILER}/{MODE}/{HELLO}/{ARCHITECTURE}/hello{DISTRIBUTION}world");
            OptionsStub options;
            setupBasicOptions(options, actualTargets, actualCompilers);
            addSettings(options.m_settings["bootstrap"], "filename", filename);
            addSettings(options.m_settings["bootstrap"], command2, "build-dir");
            addSettings(options.m_settings["bootstrap"][command2], "build-dir", buildDir);
            addSettings(options.m_settings["bootstrap"][command2], "patterns", {"COMPILER", "MODE", "ARCHITECTURE", "DISTRIBUTION"});
            ExecutorStub executor;
            options.setExecutor(&executor);

            Bootstrap plugin;

            WHEN("We use the init plugin") {
                Task task;
                REQUIRE(plugin.apply(command1, task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& compiler : actualCompilers) {
                        Task expectedTask;
                        expectedTask.append(TaskCollection({"./" + filename}));
                        expectedQueue.push_back(expectedTask);
                    }
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
            WHEN("We use the other-init plugin") {
                Task task;
                REQUIRE(plugin.apply(command2, task, options) == true);

                THEN("We should get the expected task") {
                    ExecutorStub::TaskQueue expectedQueue;
                    for(const auto& compiler : actualCompilers) {
                        string compilerName = compiler.getCompiler().getName();
                        string modeName = compiler.getMode().getMode();
                        string architectureName = compiler.getArchitecture().getArchitecture();
                        string distributionName = compiler.getDistribution().getDistribution();

                        Task expectedTask;
                        expectedTask.append(TaskCollection({"cd", "build/" + compilerName + "/" + modeName + "/{HELLO}/" + architectureName + "/hello" + distributionName + "world", "&&", "./" + filename}));
                        expectedQueue.push_back(expectedTask);
                    }
                    REQUIRE(expectedQueue == executor.getExecutedTasks());
                }
            }
        }
    }
} } }
