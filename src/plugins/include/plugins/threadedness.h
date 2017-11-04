#ifndef THREADEDNESS_INCLUDE
#define THREADEDNESS_INCLUDE

#include <string>

#include <gsl/string_span>

#include "config/commandLineOptions.h"

namespace execHelper {
namespace plugins {
using Jobs = config::Jobs_t;

const gsl::czstring<> JOBS_KEY = "jobs";
} // namespace plugins
} // namespace execHelper

#endif /* THREADEDNESS_INCLUDE */
