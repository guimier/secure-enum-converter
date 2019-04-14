#include "assertions.h"
#include "secureenumconverter.h"

enum class A { A1, A2 };
enum class B { B1, B2, B3 };
using SUT = SecureEnumConverter<A, B>;

#define SEC_INTERNAL A
#define SEC_EXTERNAL B
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B1) \
    SEC_EQUIV(A::A2, B::B2) \
    SEC_ORPHAN_EXT(B::B3)
#include "secureenumconverter.inc"

START_TEST(ExternalOrphan)
    // toExternalOpt
    COMPARE_EQ(SUT::toExternalOpt(A::A1), B::B1);
    COMPARE_EQ(SUT::toExternalOpt(A::A2), B::B2);

    // toInternalOpt
    COMPARE_EQ(SUT::toInternalOpt(B::B1), A::A1);
    COMPARE_EQ(SUT::toInternalOpt(B::B2), A::A2);
    COMPARE_EQ(SUT::toInternalOpt(B::B3), std::nullopt);

    // toExternalOrThrow
    COMPARE_EQ(SUT::toExternalOrThrow(A::A1), B::B1);
    COMPARE_EQ(SUT::toExternalOrThrow(A::A2), B::B2);

    // toInternalOrThrow
    COMPARE_EQ(SUT::toInternalOrThrow(B::B1), A::A1);
    COMPARE_EQ(SUT::toInternalOrThrow(B::B2), A::A2);
    THROWS(std::invalid_argument, SUT::toInternalOrThrow(B::B3));

    // internalValues
    std::set<A> expectedInternalValues { A::A1, A::A2 };
    COMPARE_EQ(SUT::internalValues(), expectedInternalValues);

    // internalValues
    std::set<B> expectedExternalValues { B::B1, B::B2 };
    COMPARE_EQ(SUT::externalValues(), expectedExternalValues);
END_TEST
