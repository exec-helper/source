#ifndef __COMMAND_LINE_COMMAND_HPP
#define __COMMAND_LINE_COMMAND_HPP

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        class CommandLineCommand : public Plugin {
            public:
                virtual bool apply(const std::string& command, core::Task& task, const core::Options& options) const noexcept override;
        };
    }
}

#endif /* __COMMAND_LINE_COMMAND_HPP */
