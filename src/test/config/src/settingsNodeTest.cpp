#include <catch.hpp>
#include <vector>
#include <string>

#include "config/settingsNode.h"

using std::vector;
using std::string;

using execHelper::config::SettingsNode;

namespace execHelper { namespace core { namespace test {
    SCENARIO("Testing the array subscript operator", "[settingsnode]") {
        GIVEN("A settings node structure") {
            SettingsNode rootNode;
            rootNode.key = "<root>";

            SettingsNode level1aNode;
            string level1aNodeKey("level1a");
            level1aNode.key = level1aNodeKey;

            SettingsNode level1bNode;
            string level1bNodeKey("level1b");
            level1bNode.key = level1bNodeKey;

            SettingsNode level2Node;
            string level2NodeKey("level2");
            level2Node.key = level2NodeKey;

            // Build the settings node
            level1aNode.values.push_back(level2Node);

            rootNode.values.push_back(level1aNode);
            rootNode.values.push_back(level1bNode);

            WHEN("We apply the array subscript operator") {
                THEN("We should find the same values") {
                    REQUIRE(rootNode["wrong-key"] == rootNode);
                    REQUIRE(rootNode[level1aNodeKey] == level1aNode);
                    REQUIRE(rootNode[level1aNodeKey][level2NodeKey] == level2Node);
                    REQUIRE(rootNode[level1bNodeKey] == level1bNode);
                }
            }
        }
    }

    SCENARIO("Testing the to string collection method", "[settingsnode]") {
        GIVEN("A settings node structure") {
            SettingsNode rootNode;
            rootNode.key = "<root>";

            SettingsNode level1aNode;
            string level1aNodeKey("level1a");
            level1aNode.key = level1aNodeKey;

            SettingsNode level1bNode;
            string level1bNodeKey("level1b");
            level1bNode.key = level1bNodeKey;

            SettingsNode level2Node;
            string level2NodeKey("level2");
            level2Node.key = level2NodeKey;

            // Build the settings node
            level1aNode.values.push_back(level2Node);

            rootNode.values.push_back(level1aNode);
            rootNode.values.push_back(level1bNode);

            WHEN("We compare calls of toString of different nodes") {
                THEN("We should find the correct strings") {
                    REQUIRE(rootNode.toStringCollection() == vector<string>({level1aNodeKey, level1bNodeKey}));
                    REQUIRE(rootNode[level1aNodeKey].toStringCollection() == vector<string>({level2NodeKey}));
                }
            }
        }
    }

} } }
