#include <iostream>

#include "secureenumconverter.h"

enum class A { A };
enum class B { B };

using SUT = SecureEnumConverter<A, B>;

int main () {}

#define SEC_PROJ_I2E

#define SEC_INTERNAL A
#define SEC_EXTERNAL B
#define SEC_MAPPING \
    SEC_EQUIV(A::A, B::B)
#include "secureenumconverter.inc"
