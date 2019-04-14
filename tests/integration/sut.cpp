#include "sut.h"

#define SEC_INTERNAL A
#define SEC_EXTERNAL B
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B1) \
    SEC_EQUIV(A::A2, B::B2)
#include "secureenumconverter.inc"
