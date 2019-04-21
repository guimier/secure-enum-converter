#include <iostream>
#include <string>

#include "lguim/secureenumconverter.h"

enum class A { A };
enum class B { B };

using SUT1 = lguim::SecureEnumConverter<std::string, A>;
using SUT2 = lguim::SecureEnumConverter<std::string, B>;

int main () {}

#define SEC_TYPE SUT1
#define SEC_NO_SWITCH_INTERNAL
#define SEC_MAPPING \
    SEC_EQUIV("A", A::A)
#include "lguim/secureenumconverter.inc"

#define SEC_TYPE SUT2
#define SEC_MAPPING \
    SEC_EQUIV("B", B::B)
#include "lguim/secureenumconverter.inc"
