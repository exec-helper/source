#ifndef PLUGINS_GENERATORS_INCLUDE
#define PLUGINS_GENERATORS_INCLUDE

#include <memory>
#include <string>
#include <vector>

#include <rapidcheck.h>

#include "plugins/bootstrap.h"
#include "plugins/executePlugin.h"
#include "plugins/make.h"

namespace rc {

template <> struct Arbitrary<std::shared_ptr<execHelper::plugins::Plugin>> {
    static Gen<std::shared_ptr<execHelper::plugins::Plugin>> arbitrary() {
        static auto& pluginNames =
            execHelper::plugins::ExecutePlugin::getPluginNames();
        return gen::map(
            gen::elementOf(pluginNames), [](const std::string& name) {
                return std::shared_ptr<execHelper::plugins::Plugin>(
                    execHelper::plugins::ExecutePlugin::getPlugin(name));
            });
    };
};

template <> struct Arbitrary<std::unique_ptr<execHelper::plugins::Plugin>> {
    static Gen<std::unique_ptr<execHelper::plugins::Plugin>> arbitrary() {
        static auto& pluginNames =
            execHelper::plugins::ExecutePlugin::getPluginNames();
        return gen::map(
            gen::elementOf(pluginNames), [](const std::string& name) {
                return std::unique_ptr<execHelper::plugins::Plugin>(
                    execHelper::plugins::ExecutePlugin::getPlugin(name));
            });
    };
};

} // namespace rc

#endif /* PLUGINS_GENERATORS_INCLUDE */
