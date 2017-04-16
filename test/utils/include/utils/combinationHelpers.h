#ifndef COMBINATION_HELPERS_INCLUDE
#define COMBINATION_HELPERS_INCLUDE

#include <initializer_list>

#include "config/settingsNode.h"
#include "core/task.h"

namespace execHelper {
    namespace test {
        namespace combinationHelpers {
            void setEnvironment(const config::SettingsNode::SettingsKeys& baseSettingsKeys, core::Task* expectedTask, config::SettingsNode* rootSettings, const std::initializer_list<core::EnvironmentValue>& values) noexcept;
        } // combinationHelpers
    } // test
} // execHelper


#endif  /* COMBINATION_HELPERS_INCLUDE */
