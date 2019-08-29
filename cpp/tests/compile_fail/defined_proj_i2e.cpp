#include <iostream>

#include "lguim/secureenumconverter.h"

enum class A { A };
enum class B { B };

using SUT = lguim::SecureEnumConverter<A, B>;

int main () {}

#define SEC_PROJ_I2E

#define SEC_TYPE SUT
#define SEC_MAPPING \
    SEC_EQUIV(A::A, B::B)
#include "lguim/secureenumconverter.inc"
