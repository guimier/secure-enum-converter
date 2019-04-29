#include <iostream>
#include <sstream>

#define START_TEST(NAME)                                      \
    int main() {                                              \
        struct {                                              \
            int ok, nok;                                      \
            const char* name;                                 \
            std::ostringstream errors;                        \
        } tst_status = { 0, 0, #NAME, std::ostringstream{} }; \
                                                              \
        auto tst_fails = [&](bool res) {                      \
            if (res) {                                        \
                ++tst_status.ok;                              \
            } else {                                          \
                ++tst_status.nok;                             \
            }                                                 \
            return !res;                                      \
        };                                                    \
                                                              \
        {

#define END_TEST                                                             \
        }                                                                    \
                                                                             \
        if (tst_status.nok == 0) {                                           \
            std::cout << "PASSED: Tests batch “" << tst_status.name << "” (" \
                << tst_status.ok << " tests)."                               \
                << std::endl;                                                \
        } else {                                                             \
            std::cout << "FAILED: Tests batch “" << tst_status.name << "” (" \
                << tst_status.ok << " passed; "                              \
                << tst_status.nok << " failed):"                             \
                << std::endl                                                 \
                << tst_status.errors.str();                                  \
        }                                                                    \
                                                                             \
        return tst_status.nok != 0;                                          \
    }

#define COMPARE_OP(ACTUAL, EXPECTED, OP)                     \
    do {                                                     \
        const auto& cmp_actual = (ACTUAL);                   \
        const auto& cmp_expected = (EXPECTED);               \
        if (tst_fails(cmp_actual OP cmp_expected)) {         \
            tst_status.errors                                \
                << "\tASSERTION FAILED: "                    \
                << #ACTUAL << " " << #OP << " " << #EXPECTED \
                << "(" << __FILE__ << ":" << __LINE__ << ")" \
                << std::endl;                                \
        }                                                    \
    } while (false)

#define COMPARE_EQ(ACTUAL, EXPECTED) COMPARE_OP(ACTUAL, EXPECTED, ==)

#define THROWS(TYPE, EXPR)                                         \
    do {                                                           \
        bool thr_ok = true;                                        \
        try {                                                      \
            EXPR;                                                  \
            thr_ok = false;                                        \
        } catch (TYPE&) {}                                         \
        if (tst_fails(thr_ok)) {                                   \
            tst_status.errors                                      \
                << "\tDIDN’T THROW:"                               \
                << #EXPR                                           \
                << "(" << __FILE__ << ":" << __LINE__ << ")"       \
                << std::endl;                                      \
        }                                                          \
    } while (false)
