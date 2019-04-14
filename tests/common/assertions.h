#include <iostream>

#define COMPARE_OP(ACTUAL, EXPECTED, OP) \
    do { \
        const auto& cmp_actual = (ACTUAL); \
        const auto& cmp_expected = (EXPECTED); \
        if (!(cmp_actual OP cmp_expected)) { \
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl; \
            std::cerr << "\tASSERTION FAILED: " << #ACTUAL << " " << #OP << " " << #EXPECTED << std::endl; \
            return 1; \
        } \
    } while (false)

#define COMPARE_EQ(ACTUAL, EXPECTED) COMPARE_OP(ACTUAL, EXPECTED, ==)
