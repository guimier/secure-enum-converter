#include "assertions.h"
#include "sut.h"

int main () {
    COMPARE_EQ(SUT::toExternalOpt(A::A1), B::B1);
    COMPARE_EQ(SUT::toExternalOpt(A::A2), B::B2);
}
