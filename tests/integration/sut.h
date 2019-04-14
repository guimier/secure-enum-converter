#include "secureenumconverter.h"

enum class A { A1, A2 };
enum class B { B1, B2 };
using SUT = SecureEnumConverter<A, B>;
