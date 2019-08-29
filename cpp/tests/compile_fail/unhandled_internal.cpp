#include "assertions.h"
#include "lguim/secureenumconverter.h"

enum class A { A1, A2, A3 };
enum class B { B1, B2 };
using SUT = lguim::SecureEnumConverter<A, B>;

#define SEC_TYPE SUT
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B2) \
    SEC_EQUIV(A::A2, B::B1)
#include "lguim/secureenumconverter.inc"

int main () {
}
