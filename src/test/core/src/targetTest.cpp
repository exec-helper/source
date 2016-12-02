#include <vector>
#include <string>

#include <catch.hpp>

#include "core/targetDescription.h"

using std::string;
using std::vector;

using execHelper::core::TargetDescription;

namespace {
    struct TargetParameters {
        TargetDescription::Target target; 
        TargetDescription::RunTarget runTarget;
    };
}

namespace execHelper { namespace core {
    namespace test {
        SCENARIO("Creating a target description", "[target][targetdescription]") {
            GIVEN("Certain parameters for the target description") {
                TargetDescription::TargetCollection actualTargets = {"test", "test1"};
                TargetDescription::RunTargetCollection actualRunTargets = {"bin", "unittest"};

                WHEN("We create a target description with these parameters") {
                   TargetDescription targetToTest(actualTargets, actualRunTargets); 

                    THEN("We should get the same parameters back") {
                        REQUIRE(targetToTest.getTargets() == actualTargets);
                        REQUIRE(targetToTest.getRunTargets() == actualRunTargets);
                    }
                }
            }
        }
        SCENARIO("Looping over a target description", "[target][targetdescription]") {
            GIVEN("A const target collection over which we can loop and all combinations of these") {
                TargetDescription::TargetCollection actualTargets = {"test", "test1"};
                TargetDescription::RunTargetCollection actualRunTargets = {"bin", "unittest"};
                const TargetDescription target(actualTargets, actualRunTargets); 

                vector<TargetParameters> orderedCombinations;

                // Construct the combination matrix
                for(const auto& actualTarget : actualTargets) {
                    for(const auto& runTarget : actualRunTargets) {
                        TargetParameters targetParameters;
                        targetParameters.target = actualTarget;
                        targetParameters.runTarget = runTarget;
                        orderedCombinations.push_back(targetParameters);
                    }
                }

                WHEN("We try too loop over the collection") {
                    THEN("We should be able to loop over it using iterators") {
                        size_t orderedCombinationsIndex = 0U;
                        for(TargetDescription::const_iterator it = target.begin(); it != target.end(); ++it) {
                            const TargetDescriptionElement iteratedTargetDescription = *it;
                            REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                            REQUIRE(iteratedTargetDescription.getTarget() == orderedCombinations[orderedCombinationsIndex].target);
                            REQUIRE(iteratedTargetDescription.getRunTarget() == orderedCombinations[orderedCombinationsIndex].runTarget);
                            ++orderedCombinationsIndex;
                        } 
                        REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                    }
                    THEN("We should be able to loop over it using for each loops") {
                        size_t orderedCombinationsIndex = 0U;
                        for(const auto& iteratedTargetDescription : target) {
                            REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                            REQUIRE(iteratedTargetDescription.getTarget() == orderedCombinations[orderedCombinationsIndex].target);
                            REQUIRE(iteratedTargetDescription.getRunTarget() == orderedCombinations[orderedCombinationsIndex].runTarget);
                            ++orderedCombinationsIndex;
                        } 
                        REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                    }
                }
            }
            GIVEN("A non-const target collection over which we can loop and all combinations of these") {
                TargetDescription::TargetCollection actualTargets = {"test", "test1"};
                TargetDescription::RunTargetCollection actualRunTargets = {"bin", "unittest"};
                TargetDescription target(actualTargets, actualRunTargets); 

                vector<TargetParameters> orderedCombinations;

                // Construct the combination matrix
                for(const auto& actualTarget : actualTargets) {
                    for(const auto& runTarget : actualRunTargets) {
                        TargetParameters targetParameters;
                        targetParameters.target = actualTarget;
                        targetParameters.runTarget = runTarget;
                        orderedCombinations.push_back(targetParameters);
                    }
                }

                WHEN("We try too loop over the collection") {
                    THEN("We should be able to loop over it using iterators") {
                        size_t orderedCombinationsIndex = 0U;
                        for(TargetDescription::iterator it = target.begin(); it != target.end(); ++it) {
                            const TargetDescriptionElement iteratedTargetDescription = *it;
                            REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                            REQUIRE(iteratedTargetDescription.getTarget() == orderedCombinations[orderedCombinationsIndex].target);
                            REQUIRE(iteratedTargetDescription.getRunTarget() == orderedCombinations[orderedCombinationsIndex].runTarget);
                            ++orderedCombinationsIndex;
                        } 
                        REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                    }
                    THEN("We should be able to loop over it using for each loops") {
                        size_t orderedCombinationsIndex = 0U;
                        for(auto iteratedTargetDescription : target) {
                            REQUIRE(orderedCombinationsIndex < orderedCombinations.size());
                            REQUIRE(iteratedTargetDescription.getTarget() == orderedCombinations[orderedCombinationsIndex].target);
                            REQUIRE(iteratedTargetDescription.getRunTarget() == orderedCombinations[orderedCombinationsIndex].runTarget);
                            ++orderedCombinationsIndex;
                        } 
                        REQUIRE(orderedCombinationsIndex == orderedCombinations.size());
                    }
                }
            }

        }
        SCENARIO("Special cases for target iteration", "[target][targetdescription]") {
            GIVEN("Empty collections for the target") {
                TargetDescription::TargetCollection actualTargets = {};
                TargetDescription::RunTargetCollection actualRunTargets = {};
                const TargetDescription target(actualTargets, actualRunTargets); 

                WHEN("We get the begin iterator") {
                    THEN("The begin iterator should be equal to the end iterator") {
                        REQUIRE(target.begin() == target.end());
                    }
                }
            }
        }

        SCENARIO("Test the comparisons of the targets", "[target][targetdescription]") {
            GIVEN("One test target") {
                TargetDescription actualTarget({"target1", "target2"}, {"runTarget1", "runTarget2"});

                WHEN("We compare the target with itself") {
                    THEN("We should get it to be equal") {
                        REQUIRE(actualTarget == actualTarget);
                        REQUIRE_FALSE(actualTarget != actualTarget);
                    }
                }
            }
            GIVEN("Two different targets with the same content") {
                TargetDescription target1({"target1", "target2"}, {"runTarget1", "runTarget2"});
                TargetDescription target2({"target1", "target2"}, {"runTarget1", "runTarget2"});

                WHEN("We compare the targets") {
                    THEN("We should get them to be equal") {
                        REQUIRE(target1 == target2);
                        REQUIRE_FALSE(target1 != target2);
                    }
                }
            }
            GIVEN("Two different targets with a different content") {
                TargetDescription target1({"target1", "target2"}, {"runTarget1", "runTarget2"});
                TargetDescription target2({"target1"}, {"runTarget1", "runTarget2"});
                TargetDescription target3({"target1", "target2"}, {"runTarget1"});
                TargetDescription target4({}, {});
                TargetDescription target5({"target1", "target2"}, {});
                TargetDescription target6({}, {"runTarget1", "runTarget2"});

                WHEN("We compare the targets") {
                    THEN("We should get them to be not equal") {
                        REQUIRE_FALSE(target1 == target2);
                        REQUIRE_FALSE(target1 == target3);
                        REQUIRE_FALSE(target2 == target3);
                        REQUIRE_FALSE(target1 == target4);
                        REQUIRE_FALSE(target1 == target5);
                        REQUIRE_FALSE(target1 == target6);

                        REQUIRE(target1 != target2);
                        REQUIRE(target1 != target3);
                        REQUIRE(target2 != target3);
                        REQUIRE(target1 != target4);
                        REQUIRE(target1 != target5);
                        REQUIRE(target1 != target6);
                    }
                }
            }
        }
    }
} }
