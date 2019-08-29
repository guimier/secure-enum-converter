#include <iostream>

#include "lguim/secureenumconverter.h"

enum class A { A };
enum class B { B };

using SUT = lguim::SecureEnumConverter<A, B>;

int main () {}

#define SEC_TYPE SUT
#include "lguim/secureenumconverter.inc"
