#include "lguim/secureenumconverter.h"

enum class A { A1, A2 };
enum class B { B1, B2 };
enum class C {};
using SUT = lguim::TypedEnumConverter<A, B>;

#define SEC_TYPE SUT
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B1) \
    SEC_EQUIV(A::A2, B::B2)
#include "lguim/secureenumconverter.inc"

int main() {
    const auto& out = SUT::convertOpt<C>(A::A1);
}
