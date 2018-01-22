#include <catch2/catch.hpp>

#define THEN_WHEN(x)
#define THEN_CHECK(x)

#define MAKE_COMBINATIONS(desc)                                                \
    uint32_t NUMBER_OF_COMBINATIONS = 0;                                       \
    const uint32_t CURRENT_COUNT = __COUNTER__;                                \
    for(uint32_t make_combinations_index = 0U;                                 \
        (make_combinations_index < (1U << NUMBER_OF_COMBINATIONS) ||           \
         make_combinations_index == 0U);                                       \
        ++make_combinations_index)

#define COMBINATION                                                            \
    if(make_combinations_index == 0) {                                         \
        ++NUMBER_OF_COMBINATIONS;                                              \
    }                                                                          \
    if(make_combinations_index &                                               \
       (1U << (uint32_t(__COUNTER__) - CURRENT_COUNT - 1U)))

#define COMBINATIONS(desc)                                                     \
    if(make_combinations_index == 0) {                                         \
        ++NUMBER_OF_COMBINATIONS;                                              \
    }                                                                          \
    if(make_combinations_index &                                               \
       (1U << (uint32_t(__COUNTER__) - CURRENT_COUNT - 1U)))

#define COMBINATIONS_NO_BRANCH_TAKEN if(make_combinations_index <= 1U)
#define COMBINATIONS_ONLY_IF_BRANCH_TAKEN if(make_combinations_index > 1U)
