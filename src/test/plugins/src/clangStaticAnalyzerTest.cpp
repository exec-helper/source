#include <catch.hpp>

#include "plugins/clangStaticAnalyzer.h"

#include "executorStub.h"
#include "optionsStub.h"

#include "core/pattern.h"

#include "utils/utils.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::ExecutorInterface;
using execHelper::core::test::ExecutorStub;
using execHelper::core::Task;
using execHelper::core::TaskCollection;
using execHelper::core::Pattern;

using execHelper::test::utils::addSettings;
using execHelper::test::OptionsStub;
using execHelper::test::utils::TargetUtil;
using execHelper::test::utils::CompilerUtil;
using execHelper::test::utils::getAllPatterns;
using execHelper::test::utils::getAllPatternKeys;
using execHelper::test::utils::Patterns;

namespace {
    void setupBasicOptions(OptionsStub& options, const Patterns& patterns) {
        addSettings(options.m_settings, "commands", {"analyze", "analyze2"});
        addSettings(options.m_settings, "analyze", {"clang-static-analyzer"});
        addSettings(options.m_settings, "analyze2", {"clang-static-analyzer"});

        for(const auto& pattern : patterns) {
            options.m_patternsHandler->addPattern(pattern);
        }
    }
}

namespace execHelper { namespace plugins { namespace test {
    SCENARIO("Testing the default options of the clangStaticAnalyzer plugin", "[plugins][clangStaticAnalyzer]") {
        GIVEN("A clangStaticAnalyzer plugin object and basic settings") {
            OptionsStub options;
            TargetUtil targetUtil;
            CompilerUtil compilerUtil;
            Patterns patterns = getAllPatterns({targetUtil, compilerUtil});
            setupBasicOptions(options, patterns);

            ClangStaticAnalyzer plugin;
            Task task;

            WHEN("We use the plugin") {
                THEN("It should fail") {
                    REQUIRE(plugin.apply("random-command", task, options) == false);
                }
            }
            WHEN("We add both the build and clean command with no content") {
                SettingsNode& rootSettings = options.m_settings;
                addSettings(rootSettings, "clang-static-analyzer", "build-command");

                THEN("It should fail") {
                    REQUIRE(plugin.apply("random-command", task, options) == false);
                }
            }
            WHEN("We add the build command correctly") {
                SettingsNode& rootSettings = options.m_settings;
                addSettings(rootSettings, "clang-static-analyzer", "build-command");
                addSettings(rootSettings["clang-static-analyzer"], "build-command", "memory");

                THEN("It should succeed") {
                    REQUIRE(plugin.apply("random-command", task, options) == true);
                }
            }
        }
    }
} } }
