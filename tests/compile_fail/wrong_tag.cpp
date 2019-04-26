#include "lguim/secureenumconverter.h"

enum class A { A1, A2 }; struct TA;
enum class B { B1, B2 }; struct TB;
struct TC;
using SUT = lguim::TaggedEnumConverter<TA, A, TB, B>;

#define SEC_TYPE SUT
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B1) \
    SEC_EQUIV(A::A2, B::B2)
#include "lguim/secureenumconverter.inc"

int main() {
    const auto& out = SUT::convertOpt<TC>(A::A1);
}
