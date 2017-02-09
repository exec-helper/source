#include <catch.hpp>

#define THEN_WHEN(x)
#define THEN_CHECK(x)

#define MAKE_COMBINATIONS(desc) unsigned int NUMBER_OF_COMBINATIONS = 0; \
                                const unsigned int CURRENT_COUNT = __COUNTER__; \
                                for(unsigned int i = 0U; (i < (1U << NUMBER_OF_COMBINATIONS) || i == 0U); ++i)
#define COMBINATION    if(i == 0) { ++NUMBER_OF_COMBINATIONS;} \
                       if(i & (1 << (__COUNTER__ - CURRENT_COUNT - 1U)))

#define COMBINATIONS(desc)    if(i == 0) { ++NUMBER_OF_COMBINATIONS;} \
                              if(i & (1 << (__COUNTER__ - CURRENT_COUNT - 1U)))
