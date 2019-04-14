#include "secureenumconverter.h"

#ifdef SEC_EQUIV
    #error "SEC_EQUIV defined before including secureenumconverter.inc"
#endif

#ifdef SEC_PROJ_I2E
    #error "SEC_PROJ_I2E defined before including secureenumconverter.inc"
#endif

#ifdef SEC_PROJ_E2I
    #error "SEC_PROJ_E2I defined before including secureenumconverter.inc"
#endif

#ifdef SEC_ORPHAN_INT
    #error "SEC_ORPHAN_INT defined before including secureenumconverter.inc"
#endif

#ifdef SEC_ORPHAN_EXT
    #error "SEC_ORPHAN_EXT defined before including secureenumconverter.inc"
#endif

#ifndef SEC_INTERNAL
    #error "SEC_INTERNAL not defined"
#endif

#ifndef SEC_EXTERNAL
    #error "SEC_EXTERNAL not defined"
#endif

#ifndef SEC_MAPPING
    #error "SEC_MAPPING not defined"
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wswitch"

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::toInternalOpt(External external) -> std::optional<Internal>
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) case EXT_VAL: return INT_VAL;
    #define SEC_PROJ_I2E(INT_VAL, EXT_VAL)
    #define SEC_PROJ_E2I(INT_VAL, EXT_VAL) case EXT_VAL: return INT_VAL;
    #define SEC_ORPHAN_INT(INT_VAL)
    #define SEC_ORPHAN_EXT(EXT_VAL) case EXT_VAL: return std::nullopt;

    switch (external) {
        SEC_MAPPING
    }

    // This is unreachable if SEC_MAPPING is properly defined (which should be
    // the case if the user has no warning).
    return std::nullopt;

    #undef SEC_ORPHAN_INT
    #undef SEC_ORPHAN_EXT
    #undef SEC_PROJ_I2E
    #undef SEC_PROJ_E2I
    #undef SEC_EQUIV
}

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::toExternalOpt(Internal internal) -> std::optional<External>
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) case INT_VAL: return EXT_VAL;
    #define SEC_PROJ_I2E(INT_VAL, EXT_VAL) case INT_VAL: return EXT_VAL;
    #define SEC_PROJ_E2I(INT_VAL, EXT_VAL)
    #define SEC_ORPHAN_INT(INT_VAL) case INT_VAL: return std::nullopt;
    #define SEC_ORPHAN_EXT(EXT_VAL)

    switch (internal) {
        SEC_MAPPING
    }

    // This is unreachable if SEC_MAPPING is properly defined (which should be
    // the case if the user has no warning).
    return std::nullopt;

    #undef SEC_ORPHAN_INT
    #undef SEC_ORPHAN_EXT
    #undef SEC_PROJ_I2E
    #undef SEC_PROJ_E2I
    #undef SEC_EQUIV
}

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::convertibleInternalValues() -> const std::set<Internal>&
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) INT_VAL,
    #define SEC_PROJ_I2E(INT_VAL, EXT_VAL) INT_VAL,
    #define SEC_PROJ_E2I(INT_VAL, EXT_VAL)
    #define SEC_ORPHAN_INT(INT_VAL)
    #define SEC_ORPHAN_EXT(EXT_VAL)

    static const std::set<Internal> values {
        SEC_MAPPING
    };

    return values;

    #undef SEC_ORPHAN_INT
    #undef SEC_ORPHAN_EXT
    #undef SEC_PROJ_I2E
    #undef SEC_PROJ_E2I
    #undef SEC_EQUIV
}

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::convertibleExternalValues() -> const std::set<External>&
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) EXT_VAL,
    #define SEC_PROJ_I2E(INT_VAL, EXT_VAL)
    #define SEC_PROJ_E2I(INT_VAL, EXT_VAL) EXT_VAL,
    #define SEC_ORPHAN_INT(INT_VAL)
    #define SEC_ORPHAN_EXT(EXT_VAL)

    static const std::set<External> values {
        SEC_MAPPING
    };

    return values;

    #undef SEC_ORPHAN_INT
    #undef SEC_ORPHAN_EXT
    #undef SEC_PROJ_I2E
    #undef SEC_PROJ_E2I
    #undef SEC_EQUIV
}

#pragma GCC diagnostic pop

#undef SEC_INTERNAL
#undef SEC_EXTERNAL
#undef SEC_MAPPING
