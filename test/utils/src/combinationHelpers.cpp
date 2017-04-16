#include "combinationHelpers.h"

#include "utils.h"

#include "plugins/pluginUtils.h"

using std::initializer_list;

using execHelper::config::SettingsNode;
using execHelper::core::Task;
using execHelper::core::EnvironmentValue;
using execHelper::plugins::getEnvironmentKey;

using execHelper::test::utils::combineVectors;

namespace execHelper {
    namespace test {
        namespace combinationHelpers {
            void setEnvironment(const SettingsNode::SettingsKeys& baseSettingsKeys, Task* expectedTask, SettingsNode* rootSettings, const initializer_list<EnvironmentValue>& values) noexcept {
                for(auto envVar : values) {
                    rootSettings->add(combineVectors(baseSettingsKeys, {getEnvironmentKey(), envVar.first}), envVar.second);
                    expectedTask->appendToEnvironment(std::move(envVar));
                }
            }
        } // combinationHelpers 
    } // test
} // execHelper


