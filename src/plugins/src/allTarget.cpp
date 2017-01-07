#include "allTarget.h"

#include "config/settingsNode.h"

#include "pluginUtils.h"

using std::string;

using execHelper::config::SettingsNode;
using execHelper::core::Pattern;

namespace {
    static const string patternKey("pattern-key");
    static const string allTargetKey("all-target");
}

namespace execHelper { namespace plugins {
    AllTarget::AllTarget(const SettingsNode& allTargetSettingsNode) noexcept :
        m_patternKey(getSetting(allTargetSettingsNode, patternKey, "???")),
        m_all_value(getSetting(allTargetSettingsNode, patternKey, "???"))
    {
        ;
    }
} }
