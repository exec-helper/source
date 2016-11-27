#ifndef __COMMAND_LINE_COMMAND_HPP
#define __COMMAND_LINE_COMMAND_HPP

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        class CommandLineCommand : public Plugin {
            public:
                virtual void apply(core::Task& task, const core::ExecHelperOptions& options) const noexcept;
        };
    }
}

#endif /* __COMMAND_LINE_COMMAND_HPP */
