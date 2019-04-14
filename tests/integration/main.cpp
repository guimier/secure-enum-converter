#include "assertions.h"
#include "sut.h"

START_TEST(Integration)
    // toExternalOpt
    COMPARE_EQ(SUT::toExternalOpt(A::A1), B::B1);
    COMPARE_EQ(SUT::toExternalOpt(A::A2), B::B2);
    COMPARE_EQ(SUT::toExternalOpt(A::A3), B::B3);
    COMPARE_EQ(SUT::toExternalOpt(A::A4), std::nullopt);
    COMPARE_EQ(SUT::toExternalOpt(A::A2_old), B::B2);

    // toInternalOpt
    COMPARE_EQ(SUT::toInternalOpt(B::B1), A::A1);
    COMPARE_EQ(SUT::toInternalOpt(B::B2), A::A2);
    COMPARE_EQ(SUT::toInternalOpt(B::B3), A::A3);
    COMPARE_EQ(SUT::toInternalOpt(B::B5), std::nullopt);
    COMPARE_EQ(SUT::toInternalOpt(B::B3_old), A::A3);

    // toExternalOrThrow
    COMPARE_EQ(SUT::toExternalOrThrow(A::A1), B::B1);
    COMPARE_EQ(SUT::toExternalOrThrow(A::A2), B::B2);
    COMPARE_EQ(SUT::toExternalOrThrow(A::A3), B::B3);
    THROWS(std::invalid_argument, SUT::toExternalOrThrow(A::A4));
    COMPARE_EQ(SUT::toExternalOrThrow(A::A2_old), B::B2);

    // toInternalOrThrow
    COMPARE_EQ(SUT::toInternalOrThrow(B::B1), A::A1);
    COMPARE_EQ(SUT::toInternalOrThrow(B::B2), A::A2);
    COMPARE_EQ(SUT::toInternalOrThrow(B::B3), A::A3);
    THROWS(std::invalid_argument, SUT::toInternalOrThrow(B::B5));
    COMPARE_EQ(SUT::toInternalOrThrow(B::B3_old), A::A3);

    // internalValues
    std::set<A> expectedInternalValues { A::A1, A::A2, A::A3, A::A2_old };
    COMPARE_EQ(SUT::internalValues(), expectedInternalValues);

    // internalValues
    std::set<B> expectedExternalValues { B::B1, B::B2, B::B3, B::B3_old };
    COMPARE_EQ(SUT::externalValues(), expectedExternalValues);
END_TEST
