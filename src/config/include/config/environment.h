#ifndef ENVIRONMENT_INCLUDE
#define ENVIRONMENT_INCLUDE

#include <map>
#include <string>
#include <vector>

#include <gsl/string_span>

namespace execHelper {
namespace config {
using EnvArg = std::string;
using EnvArgs = std::vector<EnvArg>;

using EnvironmentCollection = std::map<std::string, EnvArg>;
using EnvironmentValue = std::pair<std::string, EnvArg>;

static const gsl::czstring<> ENVIRONMENT_KEY = "environment";

} // namespace config
} // namespace execHelper

#endif /* ENVIRONMENT_INCLUDE */
