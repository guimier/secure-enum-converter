#include "assertions.h"
#include "secureenumconverter.h"

enum class A { A1, A2, A3 };
enum class B { B1, B2 };
using SUT = SecureEnumConverter<A, B>;

#define SEC_TYPE SUT
#define SEC_MAPPING \
    SEC_PROJ_I2E(A::A3, B::B2) \
    SEC_EQUIV(A::A1, B::B1) \
    SEC_EQUIV(A::A2, B::B2)
#include "secureenumconverter.inc"

START_TEST(ProjectionToExternal)
    // toExternalOpt
    COMPARE_EQ(SUT::toExternalOpt(A::A1), B::B1);
    COMPARE_EQ(SUT::toExternalOpt(A::A2), B::B2);
    COMPARE_EQ(SUT::toExternalOpt(A::A3), B::B2);

    // toInternalOpt
    COMPARE_EQ(SUT::toInternalOpt(B::B1), A::A1);
    COMPARE_EQ(SUT::toInternalOpt(B::B2), A::A2);

    // toExternalOrThrow
    COMPARE_EQ(SUT::toExternalOrThrow(A::A1), B::B1);
    COMPARE_EQ(SUT::toExternalOrThrow(A::A2), B::B2);
    COMPARE_EQ(SUT::toExternalOrThrow(A::A3), B::B2);

    // toInternalOrThrow
    COMPARE_EQ(SUT::toInternalOrThrow(B::B1), A::A1);
    COMPARE_EQ(SUT::toInternalOrThrow(B::B2), A::A2);

    // convertibleInternalValues
    std::set<A> expectedInternalValues { A::A1, A::A2, A::A3 };
    COMPARE_EQ(SUT::convertibleInternalValues(), expectedInternalValues);

    // convertibleInternalValues
    std::set<B> expectedExternalValues { B::B1, B::B2 };
    COMPARE_EQ(SUT::convertibleExternalValues(), expectedExternalValues);
END_TEST