#ifndef __PMD_H__
#define __PMD_H__

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        class Pmd : public Plugin {
            public:
                virtual bool apply(const std::string& command, core::Task& task, const core::Options& options) const noexcept override;
        };
    }
}

#endif /* __PMD_H__ */
