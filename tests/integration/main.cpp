#include "assertions.h"
#include "sut.h"

START_TEST(Integration)
    COMPARE_EQ(SUT::toExternalOpt(A::A1), B::B1);
    COMPARE_EQ(SUT::toExternalOpt(A::A2), B::B2);
END_TEST
