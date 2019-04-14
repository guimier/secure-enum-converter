#include <iostream>

#define START_TEST(NAME)                                                        \
    int main() {                                                                \
        struct { int ok, nok; const char* name; } tst_status = { 0, 0, #NAME }; \
        auto tst_fails = [&](bool res) {                                        \
            if (res) {                                                          \
                ++tst_status.ok;                                                \
            } else {                                                            \
                ++tst_status.nok;                                               \
            }                                                                   \
            return !res;                                                        \
        };                                                                      \
        {

#define END_TEST                                                   \
        }                                                          \
        std::cout << "Test report for " << tst_status.name << ": " \
            << tst_status.ok << " passed; "                        \
            << tst_status.nok << " failed."                        \
            << std::endl;                                          \
    }

#define COMPARE_OP(ACTUAL, EXPECTED, OP)                                                                   \
    do {                                                                                                   \
        const auto& cmp_actual = (ACTUAL);                                                                 \
        const auto& cmp_expected = (EXPECTED);                                                             \
        if (tst_fails(cmp_actual OP cmp_expected)) {                                                       \
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;                                         \
            std::cerr << "\tASSERTION FAILED: " << #ACTUAL << " " << #OP << " " << #EXPECTED << std::endl; \
        }                                                                                                  \
    } while (false)

#define COMPARE_EQ(ACTUAL, EXPECTED) COMPARE_OP(ACTUAL, EXPECTED, ==)

#define THROWS(TYPE, EXPR)                                         \
    do {                                                           \
        bool thr_ok = true;                                        \
        try {                                                      \
            EXPR;                                                  \
            thr_ok = false;                                        \
        } catch (TYPE) {}                                          \
        if (tst_fails(thr_ok)) {                                   \
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl; \
            std::cerr << "\tDIDN’T THROW: " << #EXPR << std::endl; \
        }                                                          \
    } while (false)
