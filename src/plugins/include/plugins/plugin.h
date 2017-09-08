#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "core/options.h"

namespace execHelper {
    namespace core {
        class Task;
        class Options;
    } // namespace core
} // namespace execHelper

namespace execHelper {
    namespace plugins {
        /**
         * \brief Interface declaration to which plugins should comply
         */
        class Plugin {
            public: 
		/*! @copydoc core::Argv::Argv(const Argv&)
		 */
                Plugin(const Plugin& other) = delete;

		/*! @copydoc core::Argv::Argv(Argv&&)
		 */
                Plugin(Plugin&& other) noexcept = delete;
                virtual ~Plugin() = default;

		/*! @copydoc core::Argv::operator=(const Argv&)
		 */
                Plugin& operator=(const Plugin& other) = delete;

		/*! @copydoc core::Argv::operator=(Argv&&)
		 */
                Plugin& operator=(Plugin&& other) noexcept = delete;

                /**
                 * Apply the plugin
                 *
                 * \param[in] command   The command that is being executed
                 * \param[in] task      The task to extend
                 * \param[in] options   The options that were passed
                 * \returns True    If the application was successful
                 *          False   Otherwise
                 */
                virtual bool apply(const core::Command& command, core::Task task, const core::Options& options) const noexcept = 0;

            protected:
                Plugin() = default;
        };

        /**
         * Register a finished task to an executor
         *
         * \param[in] task  The finished task
         * \param[in] options   The options that were passed
         * \returns True    If the task was successfully registered
         *          False   Otherwise
         */
        bool registerTask(const core::Task& task, const core::Options& options) noexcept;
    } // namespace plugins
} // namespace execHelper

#endif /* __PLUGIN_H__ */
