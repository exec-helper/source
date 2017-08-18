#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <tuple>
#include <vector>

#include "core/options.h"
#include "core/task.h"

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        struct Memory_t {
            const core::Command command;
            const core::Task* const taskAddress;
            const core::Task task;
            const core::Options* const options;

            Memory_t(core::Command aCommand, const core::Task* const aTaskAddress, core::Task aTask, const core::Options* const aOptions) :
              command(std::move(aCommand)),
              taskAddress(aTaskAddress),
              task(std::move(aTask)),
              options(aOptions)
          {
            ;
          }
        };

        class Memory : public Plugin {
            public:
                using Memories = std::vector<Memory_t>;

                Memory() = default;
                Memory(const Memory& other) = delete;
                Memory(Memory&& other) noexcept = delete;
                ~Memory() override = default;

                Memory& operator=(const Memory& other) = delete;
                Memory& operator=(Memory&& other) noexcept = delete;

                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;
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
                using Memories = Memory::Memories;

                MemoryHandler();
                MemoryHandler(const MemoryHandler& other) = delete;
                MemoryHandler(MemoryHandler&& other) noexcept = delete;
                ~MemoryHandler() override;

                MemoryHandler& operator=(const MemoryHandler& other) = delete;
                MemoryHandler& operator=(MemoryHandler&& other) noexcept = delete;

                const Memories& getExecutions() const noexcept;
                void reset() noexcept;
                void setReturnCode(bool returnCode) noexcept;
        };
    }
}

#endif  /* __MEMORY_H__ */
