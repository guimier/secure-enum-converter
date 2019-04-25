#ifndef SUT_H
#define SUT_H

#include "assertions.h"
#include "lguim/secureenumconverter.h"

enum class A { A1, A2, A3, A4, A2_old };
enum class B { B1, B2, B3, B5, B3_old };
using SUT = lguim::SecureEnumConverter<A, B>;

#endif // SUT_H
