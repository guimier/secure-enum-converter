#include "assertions.h"
#include "lguim/secureenumconverter.h"

enum class A { A1, A2 };
enum class B { B1, B2 };
using SUT = lguim::TypedEnumConverter<A, B>;

#define SEC_TYPE SUT
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B1) \
    SEC_EQUIV(A::A2, B::B2)
#include "lguim/secureenumconverter.inc"

START_TEST(Typed)
    // toExternalOpt
    COMPARE_EQ(SUT::convertOpt<B>(A::A1), B::B1);
    COMPARE_EQ(SUT::convertOpt<B>(A::A2), B::B2);

    // toInternalOpt
    COMPARE_EQ(SUT::convertOpt<A>(B::B1), A::A1);
    COMPARE_EQ(SUT::convertOpt<A>(B::B2), A::A2);

    // toExternalOrThrow
    COMPARE_EQ(SUT::convertOrThrow<B>(A::A1), B::B1);
    COMPARE_EQ(SUT::convertOrThrow<B>(A::A2), B::B2);

    // toInternalOrThrow
    COMPARE_EQ(SUT::convertOrThrow<A>(B::B1), A::A1);
    COMPARE_EQ(SUT::convertOrThrow<A>(B::B2), A::A2);

    // convertibleInternalValues
    std::set<A> expectedInternalValues { A::A1, A::A2 };
    COMPARE_EQ(SUT::convertibleValues<A>(), expectedInternalValues);

    // convertibleInternalValues
    std::set<B> expectedExternalValues { B::B1, B::B2 };
    COMPARE_EQ(SUT::convertibleValues<B>(), expectedExternalValues);
END_TEST
