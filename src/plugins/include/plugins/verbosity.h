#ifndef VERBOSITY_INCLUDE
#define VERBOSITY_INCLUDE

#include <gsl/string_span>

namespace execHelper {
namespace plugins {
using Verbosity = bool;

const gsl::czstring<> VERBOSITY_KEY = "verbose";
} // namespace plugins
} // namespace execHelper

#endif /* VERBOSITY_INCLUDE */