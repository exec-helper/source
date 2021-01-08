#ifndef LUA_PLUGIN_INCLUDE
#define LUA_PLUGIN_INCLUDE

#include <filesystem>
#include <string_view>

#include "core/task.h"

namespace execHelper::config {
class SettingsNode;
using VariablesMap = SettingsNode;
} // namespace execHelper::config

namespace execHelper::plugins {
class ExecutionContext;

[[nodiscard]] auto
luaPlugin(core::Task task, const config::VariablesMap& config,
          const ExecutionContext& context, const std::filesystem::path& script)
    -> core::Tasks;

[[nodiscard]] inline auto
luaPluginSummary(const std::filesystem::path& script) noexcept {
    using namespace std::literals;
    return "Lua plugin for module " + script.string();
}
} // namespace execHelper::plugins

#endif /* LUA_PLUGIN_INCLUDE */
