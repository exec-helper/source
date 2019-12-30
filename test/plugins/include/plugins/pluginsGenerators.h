#ifndef PLUGINS_GENERATORS_INCLUDE
#define PLUGINS_GENERATORS_INCLUDE

#include <memory>
#include <string>
#include <vector>

#include <rapidcheck.h>

#include "plugins/executePlugin.h"

namespace rc {

template <>
struct Arbitrary<std::shared_ptr<const execHelper::plugins::Plugin>> {
    static Gen<std::shared_ptr<const execHelper::plugins::Plugin>> arbitrary() {
        const auto pluginNames =
            execHelper::plugins::ExecutePlugin::getPluginNames();
        return gen::map(
            gen::elementOf(pluginNames), [](const std::string& name) {
                return std::shared_ptr<const execHelper::plugins::Plugin>(
                    execHelper::plugins::ExecutePlugin::getPlugin(name));
            });
    };
};

} // namespace rc

#endif /* PLUGINS_GENERATORS_INCLUDE */
