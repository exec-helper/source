#ifndef ASSERT_INCLUDE
#define ASSERT_INCLUDE

#ifdef TERMINATE_ON_ASSERT_FAILURE

#define GSL_THROW_ON_CONTRACT_VIOLATION

#include <exception>
#include <iostream>

#include <gsl/gsl_assert>

/**
 * Checks that an argument does not violate certain conditions (nominal programming style)
 */
#define expects(cond)   do { try{ Expects(cond); } catch(gsl::fail_fast& e) { std::cerr << e.what(); std::terminate(); } } while(false)

/**
 * Checks that an invariant still holds
 */
#define ensures(cond)   do { try{ Ensures(cond); } catch(gsl::fail_fast& e) { std::cerr << e.what(); std::terminate(); } } while(false)

#else

#define expects(cond)
#define ensures(cond)

#endif

#endif  /* ASSERT_INCLUDE */
