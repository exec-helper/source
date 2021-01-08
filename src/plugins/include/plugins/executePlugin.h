#ifndef __EXECUTE_PLUGIN_H__
#define __EXECUTE_PLUGIN_H__

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <gsl/gsl>

#include "config/commandLineOptions.h"
#include "config/pattern.h"

#include "config/variablesMap.h"
#include "executionContext.h"

namespace execHelper::plugins {
/**
 * \brief Exception thrown when the requested plugin is invalid
 *
 * Exception thrown when the requested plugin is invalid e.g. due to the fact that it can not be found
 */
struct InvalidPlugin : public std::runtime_error {
  public:
    /**
     * Create an invalid plugin
     *
     * \param[in] msg   A message detailing the specifics of the exception
     */
    inline explicit InvalidPlugin(const std::string& msg)
        : std::runtime_error(msg) {}

    /*! @copydoc InvalidPlugin(const std::string&)
     */
    inline explicit InvalidPlugin(const char* msg) : std::runtime_error(msg) {}
};

namespace detail {
/**
 * Returns a list with the names of the given plugins
 *
 * @param[in] plugins   The list of plugins to get the name for
 * @returns A list of plugin names
 */
auto getPluginNames(const Plugins& plugins) noexcept
    -> std::vector<std::string_view>;

[[nodiscard]] auto executeCommands(const config::Command& initialCommand,
                                   const config::Command& command,
                                   const core::Task& task,
                                   const ExecutionContext& context)
    -> core::Tasks;
} // namespace detail

[[nodiscard]] auto executeCommands(const config::CommandCollection& commands,
                                   const core::Task& task,
                                   const ExecutionContext& context)
    -> core::Tasks;
} // namespace execHelper::plugins

#endif /* __EXECUTE_PLUGIN_H__ */
