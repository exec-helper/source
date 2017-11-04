#ifndef __CLANG_STATIC_ANALYZER_H__
#define __CLANG_STATIC_ANALYZER_H__

#include <memory>

#include "buildPlugin.h"
#include "core/task.h"

namespace execHelper {
namespace plugins {
/**
 * \brief Plugin for running the clang static analyzer (also known as
 * scan-build)
 */
class ClangStaticAnalyzer : public BuildPlugin {
  public:
    std::string getPluginName() const noexcept override;
    config::VariablesMap getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const
        noexcept override;
    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override;
};
} // namespace plugins
} // namespace execHelper

#endif /* __CLANG_STATIC_ANALYZER_H__ */
