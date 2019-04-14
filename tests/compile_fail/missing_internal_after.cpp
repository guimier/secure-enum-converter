#include <iostream>

#include "secureenumconverter.h"

enum class A { A };
enum class B { B };

using SUT1 = SecureEnumConverter<A, B>;
using SUT2 = SecureEnumConverter<B, A>;

int main () {}

#define SEC_INTERNAL A
#define SEC_EXTERNAL B
#define SEC_MAPPING
#include "secureenumconverter.inc"

#define SEC_EXTERNAL A
#define SEC_MAPPING
#include "secureenumconverter.inc"
