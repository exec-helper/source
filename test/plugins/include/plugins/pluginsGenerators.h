#ifndef PLUGINS_GENERATORS_INCLUDE
#define PLUGINS_GENERATORS_INCLUDE

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <rapidcheck.h>

#include "plugins/executePlugin.h"
#include "plugins/pluginUtils.h"

#include "unittest/config.h"

namespace rc {

template <>
struct Arbitrary<std::shared_ptr<const execHelper::plugins::Plugin>> {
    static auto arbitrary()
        -> Gen<std::shared_ptr<const execHelper::plugins::Plugin>> {
        static const auto plugins = execHelper::plugins::discoverPlugins(
            {std::filesystem::path(PLUGINS_INSTALL_PATH)});
        return gen::map(gen::elementOf(plugins),
                        [](const auto& plugin) { return plugin.second; });
    };
};

} // namespace rc

#endif /* PLUGINS_GENERATORS_INCLUDE */
