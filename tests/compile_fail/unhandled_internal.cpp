#include "assertions.h"
#include "secureenumconverter.h"

enum class A { A1, A2, A3 };
enum class B { B1, B2 };
using SUT = SecureEnumConverter<A, B>;

#define SEC_INTERNAL A
#define SEC_EXTERNAL B
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B2) \
    SEC_EQUIV(A::A2, B::B1)
#include "secureenumconverter.inc"

int main () {
}
