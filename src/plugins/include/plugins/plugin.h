#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <functional>
#include <string>

#include "config/variablesMap.h"
#include "core/task.h"

namespace execHelper::plugins {
class ExecutionContext;

using ApplyFunction = std::function<core::Tasks(
    core::Task task, const config::VariablesMap& variables,
    const ExecutionContext& context)>;
using SummaryFunction = std::function<std::string()>;

using Plugins = std::map<std::string, ApplyFunction>;
using PluginSummaries = std::map<std::string, std::string>;

} // namespace execHelper::plugins

#endif /* __PLUGIN_H__ */
