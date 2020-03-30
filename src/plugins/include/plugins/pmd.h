#ifndef __PMD_H__
#define __PMD_H__

#include "plugin.h"

namespace execHelper {
namespace plugins {
/**
 * \brief Implements the PMD plugin
 */
class Pmd : public Plugin {
  public:
    config::VariablesMap getVariablesMap(
        const config::FleetingOptionsInterface& fleetingOptions) const
        noexcept override;
    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override;

    std::string summary() const noexcept override;
};
} // namespace plugins
} // namespace execHelper

#endif /* __PMD_H__ */
