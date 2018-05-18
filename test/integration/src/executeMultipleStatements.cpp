#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <string>

#include "base-utils/configBuilder.h"
#include "base-utils/configFileWriter.h"
#include "base-utils/directStatement.h"
#include "base-utils/execution.h"
#include "base-utils/executionContent.h"
#include "base-utils/executionHandler.h"
#include "base-utils/indirectStatement.h"
#include "base-utils/plugins.h"
#include "base-utils/testCommand.h"
#include "log/assertions.h"
#include "unittest/catch.h"

using std::make_shared;
using std::move;
using std::mt19937;
using std::ofstream;
using std::ostream;
using std::string;
using std::stringstream;
using std::to_string;
using std::uniform_int_distribution;
using std::vector;

using execHelper::test::baseUtils::ConfigBuilder;
using execHelper::test::baseUtils::ConfigFileWriter;
using execHelper::test::baseUtils::EXEC_HELPER_BINARY;
using execHelper::test::baseUtils::ExecutionContent;
using execHelper::test::baseUtils::IndirectStatement;
using execHelper::test::baseUtils::ReturnCode;
using execHelper::test::baseUtils::SUCCESS;
using execHelper::test::baseUtils::SimpleStatement;
using execHelper::test::baseUtils::Statements;
using execHelper::test::baseUtils::TestCommand;
using execHelper::test::baseUtils::YamlWriter;
using execHelper::test::baseUtils::createStatement;
using execHelper::test::baseUtils::execution::execute;

namespace execHelper {
namespace test {
namespace integration {
/**
 * Implements @ref execute-multiple-statements-predefined-order
 */
SCENARIO("Execution order of a command consisting of one predefined statement",
         "[execute-multiple-statements][execute-multiple-statements-predefined-"
         "order][scenario-execute-multiple-statements-predefined-order-"
         "single]") {
    GIVEN("A valid configuration with multiple commands consisting of one "
          "predefined, valid, successful statement") {
        const vector<string> commandStrings(
            {"command1", "command2", "command3", "command4"});
        ConfigBuilder execution;
        ConfigFileWriter config;
        YamlWriter yaml;

        for(const auto& commandString : commandStrings) {
            execution.add(TestCommand(
                commandString, {createStatement<SimpleStatement>(SUCCESS)}));
        }
        execution.write(gsl::not_null<YamlWriter*>(&yaml));
        config.write(yaml);

        WHEN("we call each of these commands separately") {
            for(const auto& command : execution) {
                auto executionIterator = execution.startIteration();
                const ReturnCode returnCode =
                    execute({EXEC_HELPER_BINARY, command.get(),
                             "--settings-file", config.getFilename()},
                            config.getDirectory());
                THEN_CHECK("the call should succeed") {
                    REQUIRE(returnCode == SUCCESS);
                }

                THEN_CHECK("the associated predefined statement should be "
                           "called exactly once") {
                    REQUIRE(command.getNumberOfStatementExecutions() == 1U);
                    for(const auto& statement : command) {
                        REQUIRE(statement->getNumberOfExecutions() == 1U);
                    }
                }

                THEN_CHECK(
                    "no other predefined statements should have been called") {
                    for(const auto& otherCommand : execution) {
                        if(command.get() != otherCommand.get()) {
                            for(const auto& statement : otherCommand) {
                                REQUIRE(statement->getNumberOfExecutions() ==
                                        0U);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * Implements @ref scenario-execute-multiple-statements-predefined-order-multiple
 */
SCENARIO("Scenario: Execution order of commands consisting of multiple "
         "predefined statements",
         "[execute-multiple-statements][execute-multiple-statements-predefined-"
         "order][scenario-execute-multiple-statements-predefined-order-"
         "multiple]") {
    GIVEN("A valid configuration with multiple commands consisting of "
          "multiple, predefined, valid, successful statements") {
        const vector<string> commandStrings(
            {"command1", "command2", "command3", "command4"});
        ConfigBuilder execution;
        ConfigFileWriter config;
        YamlWriter yaml;

        const auto NB_OF_STATEMENTS = 3U;

        for(const auto& commandString : commandStrings) {
            TestCommand command(commandString);

            for(auto i = 0U; i < NB_OF_STATEMENTS; ++i) {
                string statementKey = string(commandString)
                                          .append("-statement")
                                          .append(to_string(i));
                auto indirect = make_shared<IndirectStatement>(
                    statementKey,
                    Statements({createStatement<SimpleStatement>(SUCCESS)}));
                command.add(indirect);
            }
            execution.add(move(command));
        }
        execution.write(gsl::not_null<YamlWriter*>(&yaml));
        config.write(yaml);

        WHEN("we call each of these commands separately") {
            for(const auto& command : execution) {
                auto executionIterator = execution.startIteration();
                const ReturnCode returnCode =
                    execute({EXEC_HELPER_BINARY, command.get(),
                             "--settings-file", config.getFilename()},
                            config.getDirectory());
                THEN_CHECK("the call should succeed") {
                    REQUIRE(returnCode == SUCCESS);
                }

                THEN_CHECK("the associated predefined statements should be "
                           "called the expected number of times in the right "
                           "order") {
                    REQUIRE(command.getNbOfStatements() == NB_OF_STATEMENTS);
                    REQUIRE(command.getNumberOfStatementExecutions() ==
                            NB_OF_STATEMENTS);
                    for(const auto& statement : command) {
                        REQUIRE(statement->getNumberOfExecutions() == 1U);
                    }
                }

                THEN_CHECK("no other predefined statements except should have "
                           "been called") {
                    for(const auto& otherCommand : execution) {
                        if(command.get() != otherCommand.get()) {
                            for(const auto& statement : otherCommand) {
                                REQUIRE(statement->getNumberOfExecutions() ==
                                        0U);
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * Implements @ref scenario-execute-multiple-statements-predefined-order-duplicates
 */
SCENARIO("Duplicate configured statements",
         "[execute-multiple-statements][scenario-execute-multiple-statements-"
         "predefined-order-duplicates]") {
    GIVEN("A valid configuration with a command consisting of multiple, "
          "duplicate statements and some non-duplicate ones") {
        const vector<string> commandStrings(
            {"command1", "command2", "command3", "command4"});
        const string duplicateCommandString = "command-with-duplicates";
        const string duplicateStatementString("duplicate-statement");

        ConfigBuilder execution;
        ConfigFileWriter config;
        YamlWriter yaml;

        // Write other commands
        for(const auto& commandString : commandStrings) {
            execution.add(TestCommand(
                commandString, {createStatement<SimpleStatement>(SUCCESS)}));
        }

        // Add duplicate ones
        const auto NB_OF_DUPLICATE_STATEMENTS = 5U;
        auto duplicateStatement = make_shared<IndirectStatement>(
            duplicateStatementString,
            Statements({createStatement<SimpleStatement>(SUCCESS)}));
        TestCommand commandWithDuplicates(
            duplicateCommandString,
            Statements(NB_OF_DUPLICATE_STATEMENTS, duplicateStatement));

        // Add non-duplicate ones
        const auto NB_OF_UNIQUE_STATEMENTS = 3U;
        for(auto i = 0U; i < NB_OF_UNIQUE_STATEMENTS; ++i) {
            string statementKey = string(duplicateCommandString)
                                      .append("-statement")
                                      .append(to_string(i));
            auto indirect = make_shared<IndirectStatement>(
                statementKey,
                Statements({createStatement<SimpleStatement>(SUCCESS)}));
            commandWithDuplicates.add(indirect);
        }
        execution.add(commandWithDuplicates);

        execution.write(gsl::not_null<YamlWriter*>(&yaml));
        config.write(yaml);

        WHEN("we call this command") {
            auto iterator = execution.startIteration();
            const ReturnCode returnCode =
                execute({EXEC_HELPER_BINARY, commandWithDuplicates.get(),
                         "--settings-file", config.getFilename()},
                        config.getDirectory());

            THEN("the call should succeed") { REQUIRE(returnCode == SUCCESS); }

            THEN("the associated predefined statements should all be called "
                 "the expected number of times") {
                REQUIRE(commandWithDuplicates.getNbOfStatements() ==
                        NB_OF_UNIQUE_STATEMENTS + NB_OF_DUPLICATE_STATEMENTS);
                for(const auto& statement : commandWithDuplicates) {
                    auto expectedNumberOfExecutions = 1U;
                    if(statement->getKey() == duplicateStatementString) {
                        expectedNumberOfExecutions = NB_OF_DUPLICATE_STATEMENTS;
                    }
                    REQUIRE(statement->getNumberOfExecutions() ==
                            expectedNumberOfExecutions);
                }
            }

            THEN("no other predefined commands should have been called") {
                for(const auto& otherCommand : execution) {
                    if(commandWithDuplicates.get() != otherCommand.get()) {
                        for(const auto& statement : otherCommand) {
                            REQUIRE(statement->getNumberOfExecutions() == 0U);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Implements @ref scenario-execute-multiple-statements-predefined-order-failing-statements
 */
SCENARIO("Execute multiple statements: predefined order: failing statements",
         "[execute-multiple-statements][scenario-execute-multiple-statements-"
         "predefined-order-failing-statements]") {
    GIVEN("A valid configuration with a command consisting of multiple, "
          "predefined, valid, successful statements and a failing statement "
          "that is not the first nor the last statement") {
        const vector<string> otherCommands(
            {"other-command1", "other-command2", "other-command3"});

        ConfigBuilder execution;
        ConfigFileWriter config;
        YamlWriter yaml;

        for(const auto& commandString : otherCommands) {
            execution.add(TestCommand(
                commandString, {createStatement<SimpleStatement>(SUCCESS)}));
        }

        const auto NB_OF_STATEMENTS = 5U;
        const auto expectedReturnCode = EXIT_FAILURE;
        ensures(
            expectedReturnCode !=
            SUCCESS); // Test invariant check: the expected return code should not be equal to the success return code

        // Generate the index of the failing statement in a random yet predictable way
        ensures(
            NB_OF_STATEMENTS >=
            3); // In order to have a failing command that is not the first nor the last statement, a statement size of at least 3 statements is required
        mt19937 gen(0);
        uniform_int_distribution<> dis(
            1,
            NB_OF_STATEMENTS -
                2U); // Find a random index that will fail, that is not the first nor the last one
        auto failIndex = uint8_t(dis(gen));

        // Define the failing command
        const string commandString("failing-command");
        TestCommand failingCommand(commandString);
        for(uint8_t i = 0; i < NB_OF_STATEMENTS; ++i) {
            if(i == failIndex) {
                string statementKey = string(commandString)
                                          .append("-statement")
                                          .append(to_string(i));
                auto indirect = make_shared<IndirectStatement>(
                    statementKey, Statements({createStatement<SimpleStatement>(
                                      expectedReturnCode)}));
                failingCommand.add(indirect);
            } else {
                string statementKey = string(commandString)
                                          .append("-statement")
                                          .append(to_string(i));
                auto indirect = make_shared<IndirectStatement>(
                    statementKey,
                    Statements({createStatement<SimpleStatement>(SUCCESS)}));
                failingCommand.add(indirect);
            }
        }
        execution.add(failingCommand);

        execution.write(gsl::not_null<YamlWriter*>(&yaml));
        config.write(yaml);

        WHEN("we call this command") {
            auto iterator = execution.startIteration();
            const ReturnCode returnCode =
                execute({EXEC_HELPER_BINARY, failingCommand.get(),
                         "--settings-file", config.getFilename()},
                        config.getDirectory());

            THEN("the call should fail") { REQUIRE(returnCode != SUCCESS); }

            THEN("the call should exit with the same return code as the failed "
                 "statement") {
                REQUIRE(returnCode == expectedReturnCode);
            }

            THEN("the statements configured before the failed statement and "
                 "the failed statements should have been executed") {
                REQUIRE(failingCommand.getNumberOfStatementExecutions() ==
                        failIndex + 1U);
                for(auto i = 0U; i <= failIndex; ++i) {
                    REQUIRE(failingCommand[i]->getNumberOfExecutions() == 1U);
                }

                for(auto i = failIndex + 1U; i < failingCommand.size(); ++i) {
                    REQUIRE(failingCommand[i]->getNumberOfExecutions() == 0U);
                }
            }

            THEN("no other predefined statements should have been called") {
                for(const auto& otherCommand : execution) {
                    if(failingCommand.get() != otherCommand.get()) {
                        for(const auto& statement : otherCommand) {
                            REQUIRE(statement->getNumberOfExecutions() == 0U);
                        }
                    }
                }
            }
        }
    }
}
} // namespace integration
} // namespace test
} // namespace execHelper
