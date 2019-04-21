#include "assertions.h"
#include "lguim/secureenumconverter.h"

enum class A { A1, A2 };
enum class B { B1, B2 };
using SUT1 = lguim::SecureEnumConverter<A, B, struct Tag1>;
using SUT2 = lguim::SecureEnumConverter<A, B, struct Tag2>;

#define SEC_TYPE SUT1
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B1) \
    SEC_EQUIV(A::A2, B::B2)
#include "lguim/secureenumconverter.inc"

#define SEC_TYPE SUT2
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B2) \
    SEC_EQUIV(A::A2, B::B1)
#include "lguim/secureenumconverter.inc"

START_TEST(TwoMappings)
    // toExternalOpt
    COMPARE_EQ(SUT1::toExternalOpt(A::A1), B::B1);
    COMPARE_EQ(SUT1::toExternalOpt(A::A2), B::B2);
    COMPARE_EQ(SUT2::toExternalOpt(A::A1), B::B2);
    COMPARE_EQ(SUT2::toExternalOpt(A::A2), B::B1);

    // toInternalOpt
    COMPARE_EQ(SUT1::toInternalOpt(B::B1), A::A1);
    COMPARE_EQ(SUT1::toInternalOpt(B::B2), A::A2);
    COMPARE_EQ(SUT2::toInternalOpt(B::B1), A::A2);
    COMPARE_EQ(SUT2::toInternalOpt(B::B2), A::A1);

    // toExternalOrThrow
    COMPARE_EQ(SUT1::toExternalOrThrow(A::A1), B::B1);
    COMPARE_EQ(SUT1::toExternalOrThrow(A::A2), B::B2);
    COMPARE_EQ(SUT2::toExternalOrThrow(A::A1), B::B2);
    COMPARE_EQ(SUT2::toExternalOrThrow(A::A2), B::B1);

    // toInternalOrThrow
    COMPARE_EQ(SUT1::toInternalOrThrow(B::B1), A::A1);
    COMPARE_EQ(SUT1::toInternalOrThrow(B::B2), A::A2);
    COMPARE_EQ(SUT2::toInternalOrThrow(B::B1), A::A2);
    COMPARE_EQ(SUT2::toInternalOrThrow(B::B2), A::A1);

    // convertibleInternalValues
    std::set<A> expectedInternalValues1 { A::A1, A::A2 };
    COMPARE_EQ(SUT1::convertibleInternalValues(), expectedInternalValues1);
    std::set<A> expectedInternalValues2 { A::A1, A::A2 };
    COMPARE_EQ(SUT2::convertibleInternalValues(), expectedInternalValues2);

    // convertibleInternalValues
    std::set<B> expectedExternalValues1 { B::B1, B::B2 };
    COMPARE_EQ(SUT1::convertibleExternalValues(), expectedExternalValues1);
    std::set<B> expectedExternalValues2 { B::B1, B::B2 };
    COMPARE_EQ(SUT2::convertibleExternalValues(), expectedExternalValues2);
END_TEST
