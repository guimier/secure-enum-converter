#include <string>

#include "assertions.h"
#include "lguim/secureenumconverter.h"

enum class B { B1, B2 };
using SUT = lguim::SecureEnumConverter<std::string, B>;

#define SEC_TYPE SUT
#define SEC_NO_SWITCH_INTERNAL
#define SEC_MAPPING \
    SEC_EQUIV("B1", B::B1) \
    SEC_EQUIV("B2", B::B2)
#include "lguim/secureenumconverter.inc"

START_TEST(NoSwitchExternal)
    // toExternalOpt
    COMPARE_EQ(SUT::toExternalOpt("B1"), B::B1);
    COMPARE_EQ(SUT::toExternalOpt("B2"), B::B2);
    COMPARE_EQ(SUT::toExternalOpt("B3"), std::nullopt);

    // toInternalOpt
    COMPARE_EQ(SUT::toInternalOpt(B::B1), "B1");
    COMPARE_EQ(SUT::toInternalOpt(B::B2), "B2");

    // toExternalOrThrow
    COMPARE_EQ(SUT::toExternalOrThrow("B1"), B::B1);
    COMPARE_EQ(SUT::toExternalOrThrow("B2"), B::B2);
    THROWS(std::invalid_argument, SUT::toExternalOrThrow("B3"));

    // toInternalOrThrow
    COMPARE_EQ(SUT::toInternalOrThrow(B::B1), "B1");
    COMPARE_EQ(SUT::toInternalOrThrow(B::B2), "B2");

    // convertibleInternalValues
    std::set<std::string> expectedInternalValues { "B1", "B2" };
    COMPARE_EQ(SUT::convertibleInternalValues(), expectedInternalValues);

    // convertibleInternalValues
    std::set<B> expectedExternalValues { B::B1, B::B2 };
    COMPARE_EQ(SUT::convertibleExternalValues(), expectedExternalValues);
END_TEST

