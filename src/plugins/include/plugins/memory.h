#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <tuple>
#include <vector>

#include "plugin.h"

#include "core/task.h"
#include "core/options.h"

namespace execHelper {
    namespace plugins {
        struct Memory_t {
            const core::Command command;
            const core::Task* const taskAddress;
            const core::Task task;
            const core::Options* const options;

            Memory_t(const core::Command& aCommand, const core::Task* const aTaskAddress, const core::Task& aTask, const core::Options* const aOptions) :
              command(aCommand),
              taskAddress(aTaskAddress),
              task(aTask),
              options(aOptions)
          {
            ;
          }
        };

        class Memory : public Plugin {
            public:
                typedef std::vector<Memory_t> Memories;

                virtual bool apply(const core::Command& command, core::Task& task, const core::Options& options) const noexcept override;
            protected:
                static const Memories& getExecutions() noexcept;
                static void reset() noexcept;
                static void setReturnCode(bool returnCode) noexcept;

            private:
                static bool m_returnCode;
                static Memories m_executions;
        };

        class MemoryHandler : public Memory {
            public:
                typedef Memory::Memories Memories;

                MemoryHandler();
                ~MemoryHandler();

                const Memories& getExecutions() const noexcept;
                void reset() noexcept;
                void setReturnCode(bool returnCode) noexcept;
        };
    }
}

#endif  /* __MEMORY_H__ */
