#include <iostream>

#include "lguim/secureenumconverter.h"

enum class A { A };
enum class B { B };

using SUT1 = lguim::SecureEnumConverter<A, B>;
using SUT2 = lguim::SecureEnumConverter<B, A>;

int main () {}

#define SEC_TYPE SUT1
#define SEC_MAPPING \
    SEC_EQUIV(A::A, B::B)
#include "lguim/secureenumconverter.inc"

#define SEC_MAPPING \
    SEC_EQUIV(B::B, A::A)
#include "lguim/secureenumconverter.inc"
