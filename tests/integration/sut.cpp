#include "sut.h"

#define SEC_TYPE SUT
#define SEC_MAPPING                \
    SEC_EQUIV(A::A1, B::B1)        \
    SEC_EQUIV(A::A2, B::B2)        \
    SEC_PROJ_I2E(A::A2_old, B::B2) \
    SEC_PROJ_E2I(A::A3, B::B3_old) \
    SEC_EQUIV(A::A3, B::B3)        \
    SEC_ORPHAN_INT(A::A4)          \
    SEC_ORPHAN_EXT(B::B5)
#include "secureenumconverter.inc"
