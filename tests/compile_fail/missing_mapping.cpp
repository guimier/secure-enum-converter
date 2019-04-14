#include <iostream>

#include "secureenumconverter.h"

enum class A { A };
enum class B { B };

using SUT = SecureEnumConverter<A, B>;

int main () {}

#define SEC_TYPE SUT
#include "secureenumconverter.inc"
