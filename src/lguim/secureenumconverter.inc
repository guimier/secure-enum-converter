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

#ifndef SEC_TYPE
    #error "SEC_TYPE not defined"
#endif

#ifndef SEC_MAPPING
    #error "SEC_MAPPING not defined"
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic error "-Wswitch"

namespace lguim {

#ifndef SEC_NO_SWITCH_EXTERNAL
template <>
auto SEC_TYPE::Converter::toInternalOpt(External external) -> SEC_OPTIONAL_NS::optional<Internal>
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) case EXT_VAL: return INT_VAL;
    #define SEC_PROJ_I2E(INT_VAL, EXT_VAL)
    #define SEC_PROJ_E2I(INT_VAL, EXT_VAL) case EXT_VAL: return INT_VAL;
    #define SEC_ORPHAN_INT(INT_VAL)
    #define SEC_ORPHAN_EXT(EXT_VAL) case EXT_VAL: return SEC_OPTIONAL_NS::nullopt;

    switch (external) {
        SEC_MAPPING
    }

    // This is unreachable if SEC_MAPPING is properly defined (which should be
    // the case if the user has no warning).
    return SEC_OPTIONAL_NS::nullopt;

    #undef SEC_ORPHAN_INT
    #undef SEC_ORPHAN_EXT
    #undef SEC_PROJ_I2E
    #undef SEC_PROJ_E2I
    #undef SEC_EQUIV
}
#else // ifndef SEC_NO_SWITCH_EXTERNAL
template <>
auto SEC_TYPE::Converter::toInternalOpt(External external) -> SEC_OPTIONAL_NS::optional<Internal>
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) if (external == EXT_VAL) { return INT_VAL; } else
    #define SEC_PROJ_I2E(INT_VAL, EXT_VAL)
    #define SEC_PROJ_E2I(INT_VAL, EXT_VAL) if (external == EXT_VAL) { return INT_VAL; } else
    #define SEC_ORPHAN_INT(INT_VAL)
    #define SEC_ORPHAN_EXT(EXT_VAL) if (external == EXT_VAL) { return SEC_OPTIONAL_NS::nullopt; } else

    SEC_MAPPING {
        // This is unreachable if SEC_MAPPING is properly defined.
        return SEC_OPTIONAL_NS::nullopt;
    }

    #undef SEC_ORPHAN_INT
    #undef SEC_ORPHAN_EXT
    #undef SEC_PROJ_I2E
    #undef SEC_PROJ_E2I
    #undef SEC_EQUIV
}
#endif // ifndef SEC_NO_SWITCH_EXTERNAL

#ifndef SEC_NO_SWITCH_INTERNAL
template <>
auto SEC_TYPE::Converter::toExternalOpt(Internal internal) -> SEC_OPTIONAL_NS::optional<External>
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) case INT_VAL: return EXT_VAL;
    #define SEC_PROJ_I2E(INT_VAL, EXT_VAL) case INT_VAL: return EXT_VAL;
    #define SEC_PROJ_E2I(INT_VAL, EXT_VAL)
    #define SEC_ORPHAN_INT(INT_VAL) case INT_VAL: return SEC_OPTIONAL_NS::nullopt;
    #define SEC_ORPHAN_EXT(EXT_VAL)

    switch (internal) {
        SEC_MAPPING
    }

    // This is unreachable if SEC_MAPPING is properly defined (which should be
    // the case if the user has no warning).
    return SEC_OPTIONAL_NS::nullopt;

    #undef SEC_ORPHAN_INT
    #undef SEC_ORPHAN_EXT
    #undef SEC_PROJ_I2E
    #undef SEC_PROJ_E2I
    #undef SEC_EQUIV
}
#else // ifndef SEC_NO_SWITCH_INTERNAL
template <>
auto SEC_TYPE::Converter::toExternalOpt(Internal internal) -> SEC_OPTIONAL_NS::optional<External>
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) if (internal == INT_VAL) { return EXT_VAL; } else
    #define SEC_PROJ_I2E(INT_VAL, EXT_VAL) if (internal == INT_VAL) { return EXT_VAL; } else
    #define SEC_PROJ_E2I(INT_VAL, EXT_VAL)
    #define SEC_ORPHAN_INT(INT_VAL) if (internal == INT_VAL) { return SEC_OPTIONAL_NS::optional; } else
    #define SEC_ORPHAN_EXT(EXT_VAL)

    SEC_MAPPING {
        // This is unreachable if SEC_MAPPING is properly defined.
        return SEC_OPTIONAL_NS::nullopt;
    }

    #undef SEC_ORPHAN_INT
    #undef SEC_ORPHAN_EXT
    #undef SEC_PROJ_I2E
    #undef SEC_PROJ_E2I
    #undef SEC_EQUIV
}
#endif // ifndef SEC_NO_SWITCH_INTERNAL

template <>
auto SEC_TYPE::Converter::convertibleInternalValues() -> const std::set<Internal>&
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
auto SEC_TYPE::Converter::convertibleExternalValues() -> const std::set<External>&
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

} // namespace lguim

#pragma GCC diagnostic pop

#undef SEC_TYPE
#undef SEC_MAPPING
#undef SEC_NO_SWITCH_EXTERNAL
#undef SEC_NO_SWITCH_INTERNAL
