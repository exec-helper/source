#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <tuple>
#include <vector>

#include "core/options.h"
#include "core/task.h"

#include "plugin.h"

namespace execHelper {
    namespace plugins {
        /**
         * \brief   Remembers information that was passed to it
         */
        struct Memory_t {
            const core::Command command; //!< brief The command to remember
            const core::Task* const taskAddress; //!< brief The original address of the task. The actual task may already be destroyed when used.
            const core::Task task; //!< brief The task to remember
            const core::Options* const options; //!< brief The options to remember. The actual options may already be destroyed when used.

            /**
             * Constructor
             *
             * \param[in] aCommand  The command to remember
             * \param[in] aTaskAddress  The address of the task to remember. The lifetime of the task does not necessarily need to be guaranteed during the lifetime of this object, though it is the user's reponsibility whether it is safe to dereference it later on.
             * \param[in] aTask     The task to remember
             * \param[in] aOptions  The options to remember. The lifetime of the options object does not necessarily need to be guaranteed during the lifetime of this object, though it is the user's responsability whether it is safe to dereference it later on.
             */
            inline Memory_t(core::Command aCommand, const core::Task* const aTaskAddress, core::Task aTask, const core::Options* const aOptions) :
              command(std::move(aCommand)),
              taskAddress(aTaskAddress),
              task(std::move(aTask)),
              options(aOptions)
            {
                ;
            }
        };

        /**
         * \brief   Plugin for remembering later on what has been executed. Mainly useful for testing purposes.
         */
        class Memory : public Plugin {
            public:
                using Memories = std::vector<Memory_t>; //!< brief A collection of memories

                Memory() = default;
                Memory(const Memory& other) = delete;
                Memory(Memory&& other) noexcept = delete;
                ~Memory() override = default;

                Memory& operator=(const Memory& other) = delete;
                Memory& operator=(Memory&& other) noexcept = delete;

                bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept override;
            protected:
                /**
                 * Getter for the executions that were remembered
                 *
                 * \returns A collection of memories
                 */
                static const Memories& getExecutions() noexcept;

                /**
                 * Reset the remembered memories
                 */
                static void reset() noexcept;

                /**
                 * Set the return code for the next invocation(s) of a Memory object
                 *
                 * \param   returnCode  The code to return on the next invocation(s)
                 */
                static void setReturnCode(bool returnCode) noexcept;

            private:
                static bool m_returnCode;
                static Memories m_executions;
        };

        /**
         * \brief Makes access to a memory more accessible
         */
        class MemoryHandler : public Memory {
            public:
                using Memories = Memory::Memories; //!< brief A collection of memories

                MemoryHandler();
                MemoryHandler(const MemoryHandler& other) = delete;
                MemoryHandler(MemoryHandler&& other) noexcept = delete;
                ~MemoryHandler() override;

                MemoryHandler& operator=(const MemoryHandler& other) = delete;
                MemoryHandler& operator=(MemoryHandler&& other) noexcept = delete;

                /*! @copydoc Memory::getExecutions()
                 */
                const Memories& getExecutions() const noexcept;

                /*! @copydoc Memory::reset()
                 */
                void reset() noexcept;

                /*! @copydoc Memory::setReturnCode(bool)
                 */
                void setReturnCode(bool returnCode) noexcept;
        };
    }
}

#endif  /* __MEMORY_H__ */
