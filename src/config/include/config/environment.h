#ifndef ENVIRONMENT_INCLUDE
#define ENVIRONMENT_INCLUDE

#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace execHelper::config {
using EnvArg = std::string;
using EnvArgs = std::vector<EnvArg>;

using EnvironmentCollection = std::map<std::string, EnvArg>;
using EnvironmentValue = std::pair<std::string, EnvArg>;

using namespace std::literals;
constexpr auto ENVIRONMENT_KEY = "environment"sv;

} // namespace execHelper::config

#endif /* ENVIRONMENT_INCLUDE */
