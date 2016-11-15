#include <vector>
#include <string>
#include <memory>

#include <catch.hpp>

#include "core/execHelperOptions.h"

using std::string;
using std::vector;
using std::unique_ptr;

namespace {
    template<typename T>
    void appendVectors(T& appendTo, const T& appendFrom) {
        appendTo.insert(std::end(appendTo), std::begin(appendFrom), std::end(appendFrom));
    }

    struct MainVariables {
        int argc;
        unique_ptr<char*> argv;
    };

    MainVariables convertToMainArguments(const vector<string>& arguments) {
        MainVariables result;
        result.argc = static_cast<int>(arguments.size());
        result.argv.reset(new char*[result.argc]());

        for(size_t i = 0; i < arguments.size(); ++i) {
            result.argv.get()[i] = const_cast<char*>(arguments[i].c_str());
        }
        return result;
    }
}

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Test options with no arguments", "[execHelperOptions]") {
            GIVEN("A command line with no arguments and the default values for each parameter") {
                vector<string> arguments;
                arguments.emplace_back("UNITTEST");

                const bool default_verbosity = false;
                const CommandCollection default_commands = {};
                const TargetDescription::TargetCollection default_targets = {"all"};
                const TargetDescription::RunTargetCollection defaultRunTargets = {"all"};

                TargetDescription defaultTarget(default_targets, defaultRunTargets);

                WHEN("We parse the variables") {
                    MainVariables mainVariables = convertToMainArguments(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("We should get the default variables") {
                        REQUIRE(options.getVerbosity() == default_verbosity);
                        REQUIRE(options.getCommands() == default_commands);
                        REQUIRE(options.getTarget() == defaultTarget);
                    }
                }
            }
        }

        SCENARIO("Test options with specific arguments", "[execHelperOptions]") {
            GIVEN("The command line we want to pass using long options") {
                const CommandCollection actualCommands = {"init", "build", "run"};
                const TargetDescription actualTarget({"target1", "target2"}, {"runTarget1", "runTarget2"});

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("--verbose");
                arguments.emplace_back("--target");
                appendVectors(arguments, actualTarget.getTargets());
                arguments.emplace_back("--run-target");
                appendVectors(arguments, actualTarget.getRunTargets());

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables = convertToMainArguments(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("It should be parsed properly") {
                        REQUIRE(options.getVerbosity() == true);
                        REQUIRE(options.getCommands() == actualCommands);
                        REQUIRE(options.getTarget() == actualTarget);
                    }
                }
            }

            GIVEN("The command line we want to pass using long options") {
                const CommandCollection actualCommands = {"init", "build", "run"};
                const TargetDescription actualTarget({"target1", "target2"}, {"runTarget1", "runTarget2"});

                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                appendVectors(arguments, actualCommands);
                arguments.emplace_back("-v");
                arguments.emplace_back("-t");
                appendVectors(arguments, actualTarget.getTargets());
                arguments.emplace_back("-r");
                appendVectors(arguments, actualTarget.getRunTargets());

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables = convertToMainArguments(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("It should be parsed accordingly") {
                        REQUIRE(options.getVerbosity() == true);
                        REQUIRE(options.getCommands() == actualCommands);
                        REQUIRE(options.getTarget() == actualTarget);
                    }
                }
            }

            GIVEN("The command line arguments using the flag for the commands") {
                const CommandCollection actualCommands = {"init", "build", "run"};
                vector<string> arguments;
                arguments.emplace_back("UNITTEST");
                arguments.emplace_back("--command");
                appendVectors(arguments, actualCommands);

                WHEN("We convert it and parse the variables") {
                    MainVariables mainVariables = convertToMainArguments(arguments);
                    ExecHelperOptions options; 
                    options.parse(mainVariables.argc, mainVariables.argv.get());

                    THEN("We should get the default variables") {
                        REQUIRE(options.getCommands() == actualCommands);
                    }
                }
            }
        }
    }
} }
