#include <iostream>

#include "secureenumconverter.h"

enum class A { A };
enum class B { B };

using SUT1 = SecureEnumConverter<A, B>;
using SUT2 = SecureEnumConverter<B, A>;

int main () {}

#define SEC_TYPE SUT
#define SEC_MAPPING \
    SEC_EQUIV(A::A, B::B)
#include "secureenumconverter.inc"

#define SEC_TYPE SUT
#include "secureenumconverter.inc"
