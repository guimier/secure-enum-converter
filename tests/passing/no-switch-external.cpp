#include <string>

#include "assertions.h"
#include "lguim/secureenumconverter.h"

enum class A { A1, A2 };
using SUT = lguim::SecureEnumConverter<A, std::string>;

#define SEC_TYPE SUT
#define SEC_NO_SWITCH_EXTERNAL
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, "A1") \
    SEC_EQUIV(A::A2, "A2")
#include "lguim/secureenumconverter.inc"

START_TEST(NoSwitchExternal)
    // toExternalOpt
    COMPARE_EQ(SUT::toExternalOpt(A::A1), "A1");
    COMPARE_EQ(SUT::toExternalOpt(A::A2), "A2");

    // toInternalOpt
    COMPARE_EQ(SUT::toInternalOpt("A1"), A::A1);
    COMPARE_EQ(SUT::toInternalOpt("A2"), A::A2);
    COMPARE_EQ(SUT::toInternalOpt("A3"), std::nullopt);

    // toExternalOrThrow
    COMPARE_EQ(SUT::toExternalOrThrow(A::A1), "A1");
    COMPARE_EQ(SUT::toExternalOrThrow(A::A2), "A2");

    // toInternalOrThrow
    COMPARE_EQ(SUT::toInternalOrThrow("A1"), A::A1);
    COMPARE_EQ(SUT::toInternalOrThrow("A2"), A::A2);
    THROWS(std::invalid_argument, SUT::toInternalOrThrow("A3"));

    // convertibleInternalValues
    std::set<A> expectedInternalValues { A::A1, A::A2 };
    COMPARE_EQ(SUT::convertibleInternalValues(), expectedInternalValues);

    // convertibleInternalValues
    std::set<std::string> expectedExternalValues { "A1", "A2" };
    COMPARE_EQ(SUT::convertibleExternalValues(), expectedExternalValues);
END_TEST
