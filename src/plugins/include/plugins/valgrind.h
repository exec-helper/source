#ifndef __VALGRIND_H__
#define __VALGRIND_H__

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        class Valgrind : public Plugin {
            public:
                bool apply(const std::string& command, core::Task task, const core::Options& options) const noexcept override;
        };
    }
}

#endif /* __VALGRIND_H__ */
