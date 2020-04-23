#ifndef __VALGRIND_H__
#define __VALGRIND_H__

#include "plugin.h"

namespace execHelper {
namespace plugins {
/**
 * \brief   Plugin for running valgrind
 */
class Valgrind : public Plugin {
  public:
    config::VariablesMap
    getVariablesMap(const config::FleetingOptionsInterface& fleetingOptions)
        const noexcept override;
    bool apply(core::Task task, const config::VariablesMap& variables,
               const config::Patterns& patterns) const noexcept override;

    std::string summary() const noexcept override;
};
} // namespace plugins
} // namespace execHelper

#endif /* __VALGRIND_H__ */
