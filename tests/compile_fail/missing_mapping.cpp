#include <iostream>

#include "secureenumconverter.h"

enum class A { A };
enum class B { B };

using SUT = SecureEnumConverter<A, B>;

int main () {}

#define SEC_INTERNAL A
#define SEC_EXTERNAL B
#include "secureenumconverter.inc"
