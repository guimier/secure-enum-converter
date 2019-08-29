#include "assertions.h"
#include "lguim/secureenumconverter.h"

enum class A { A1, A2 }; struct TA;
enum class B { B1, B2 }; struct TB;
using SUT = lguim::TaggedEnumConverter<TA, A, TB, B>;

#define SEC_TYPE SUT
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B1) \
    SEC_EQUIV(A::A2, B::B2)
#include "lguim/secureenumconverter.inc"

START_TEST(Tagged)
    // toExternalOpt
    COMPARE_EQ(SUT::convertOpt<TB>(A::A1), B::B1);
    COMPARE_EQ(SUT::convertOpt<TB>(A::A2), B::B2);

    // toInternalOpt
    COMPARE_EQ(SUT::convertOpt<TA>(B::B1), A::A1);
    COMPARE_EQ(SUT::convertOpt<TA>(B::B2), A::A2);

    // toExternalOrThrow
    COMPARE_EQ(SUT::convertOrThrow<TB>(A::A1), B::B1);
    COMPARE_EQ(SUT::convertOrThrow<TB>(A::A2), B::B2);

    // toInternalOrThrow
    COMPARE_EQ(SUT::convertOrThrow<TA>(B::B1), A::A1);
    COMPARE_EQ(SUT::convertOrThrow<TA>(B::B2), A::A2);

    // convertibleInternalValues
    std::set<A> expectedInternalValues { A::A1, A::A2 };
    COMPARE_EQ(SUT::convertibleValues<TA>(), expectedInternalValues);

    // convertibleInternalValues
    std::set<B> expectedExternalValues { B::B1, B::B2 };
    COMPARE_EQ(SUT::convertibleValues<TB>(), expectedExternalValues);
END_TEST
