#include <type_traits>
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

START_TEST(TaggedHalfs)
    // Exposed {out,in}put types
    ASSERT(std::is_same<SUT::Output<TA>, A>::value);
    ASSERT(std::is_same<SUT::Input<TA>, B>::value);

    // Exposed {out,in}put types of {Reversed,}HalfConverter
    ASSERT(std::is_same<SUT::HalfConverter<TA>::Output, A>::value);
    ASSERT(std::is_same<SUT::HalfConverter<TA>::Input, B>::value);
    ASSERT(std::is_same<SUT::ReversedHalfConverter<TA>::Output, B>::value);
    ASSERT(std::is_same<SUT::ReversedHalfConverter<TA>::Input, A>::value);

    // toExternalOpt
    COMPARE_EQ(SUT::HalfConverter<TB>::convertOpt(A::A1), B::B1);
    COMPARE_EQ(SUT::HalfConverter<TB>::convertOpt(A::A2), B::B2);
    COMPARE_EQ(SUT::ReversedHalfConverter<TA>::convertOpt(A::A1), B::B1);
    COMPARE_EQ(SUT::ReversedHalfConverter<TA>::convertOpt(A::A2), B::B2);

    // toInternalOpt
    COMPARE_EQ(SUT::HalfConverter<TA>::convertOpt(B::B1), A::A1);
    COMPARE_EQ(SUT::HalfConverter<TA>::convertOpt(B::B2), A::A2);
    COMPARE_EQ(SUT::ReversedHalfConverter<TB>::convertOpt(B::B1), A::A1);
    COMPARE_EQ(SUT::ReversedHalfConverter<TB>::convertOpt(B::B2), A::A2);

    // toExternalOrThrow
    COMPARE_EQ(SUT::HalfConverter<TB>::convertOrThrow(A::A1), B::B1);
    COMPARE_EQ(SUT::HalfConverter<TB>::convertOrThrow(A::A2), B::B2);
    COMPARE_EQ(SUT::ReversedHalfConverter<TA>::convertOrThrow(A::A1), B::B1);
    COMPARE_EQ(SUT::ReversedHalfConverter<TA>::convertOrThrow(A::A2), B::B2);

    // toInternalOrThrow
    COMPARE_EQ(SUT::HalfConverter<TA>::convertOrThrow(B::B1), A::A1);
    COMPARE_EQ(SUT::HalfConverter<TA>::convertOrThrow(B::B2), A::A2);
    COMPARE_EQ(SUT::ReversedHalfConverter<TB>::convertOrThrow(B::B1), A::A1);
    COMPARE_EQ(SUT::ReversedHalfConverter<TB>::convertOrThrow(B::B2), A::A2);

    // convertibleInternalValues
    std::set<A> expectedInternalValues { A::A1, A::A2 };
    COMPARE_EQ(SUT::HalfConverter<TA>::convertibleValues(), expectedInternalValues);
    COMPARE_EQ(SUT::ReversedHalfConverter<TB>::convertibleValues(), expectedInternalValues);

    // convertibleInternalValues
    std::set<B> expectedExternalValues { B::B1, B::B2 };
    COMPARE_EQ(SUT::HalfConverter<TB>::convertibleValues(), expectedExternalValues);
    COMPARE_EQ(SUT::ReversedHalfConverter<TA>::convertibleValues(), expectedExternalValues);
END_TEST
