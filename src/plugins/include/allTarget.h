#ifndef __ALL_TARGET_H__
#define __ALL_TARGET_H__

#include "core/options.h"

namespace execHelper {
    namespace config {
        struct SettingsNode;
    }
}

namespace execHelper {
    namespace plugins {
        class AllTarget {
            public:
                AllTarget(const config::SettingsNode& allTargetSettingsNode) noexcept;
               
                const core::PatternKey m_patternKey; 
                const core::PatternValue m_all_value;
        };
    }
}

#endif  /* __ALL_TARGET_H__ */
