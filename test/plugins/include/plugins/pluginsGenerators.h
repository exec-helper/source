#ifndef PLUGINS_GENERATORS_INCLUDE
#define PLUGINS_GENERATORS_INCLUDE

#include <filesystem>

#include <rapidcheck.h>

#include "plugins/plugin.h"
#include "plugins/pluginUtils.h"

#include "unittest/config.h"

namespace rc {

template <> struct Arbitrary<execHelper::plugins::ApplyFunction> {
    static auto arbitrary() -> Gen<execHelper::plugins::ApplyFunction> {
        static const auto plugins = execHelper::plugins::discoverPlugins(
            {std::filesystem::path(PLUGINS_INSTALL_PATH)});
        return gen::map(gen::elementOf(plugins),
                        [](const auto& plugin) { return plugin.second; });
    };
};
} // namespace rc

#endif /* PLUGINS_GENERATORS_INCLUDE */
