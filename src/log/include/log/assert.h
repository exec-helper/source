#ifndef ASSERT_INCLUDE
#define ASSERT_INCLUDE

#ifdef TERMINATE_ON_ASSERT_FAILURE

#define GSL_THROW_ON_CONTRACT_VIOLATION

#include <exception>

#include <gsl/gsl>

#include "log.h"

#define expects(cond)   do { try{ Expects(cond); } catch(gsl::fail_fast& e) { LOG(e.what()); std::terminate(); } } while(false)
#define ensures(cond)   do { try{ Ensures(cond); } catch(gsl::fail_fast& e) { LOG(e.what()); std::terminate(); } } while(false)

#else

#define expects(cond)
#define ensures(cond)

#endif

#endif  /* ASSERT_INCLUDE */
