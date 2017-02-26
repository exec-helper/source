#include <catch.hpp>

#define THEN_WHEN(x)
#define THEN_CHECK(x)

#define MAKE_COMBINATIONS(desc) unsigned int NUMBER_OF_COMBINATIONS = 0; \
                                const unsigned int CURRENT_COUNT = __COUNTER__; \
                                for(unsigned int make_combinations_index = 0U; (make_combinations_index < (1U << NUMBER_OF_COMBINATIONS) || make_combinations_index == 0U); ++make_combinations_index)
#define COMBINATION    if(make_combinations_index == 0) { ++NUMBER_OF_COMBINATIONS;} \
                       if(make_combinations_index & (1 << (__COUNTER__ - CURRENT_COUNT - 1U)))

#define COMBINATIONS(desc)    if(make_combinations_index == 0) { ++NUMBER_OF_COMBINATIONS;} \
                              if(make_combinations_index & (1 << (__COUNTER__ - CURRENT_COUNT - 1U)))
