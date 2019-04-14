#include "secureenumconverter.h"

#ifndef SEC_INTERNAL
    #error "SEC_INTERNAL not defined"
#endif

#ifndef SEC_EXTERNAL
    #error "SEC_EXTERNAL not defined"
#endif

#ifndef SEC_MAPPING
    #error "SEC_MAPPING not defined"
#endif

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::toInternalOpt(External external) -> std::optional<Internal>
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) case EXT_VAL: return INT_VAL;

    switch (external) {
        SEC_MAPPING
    }

    // This is unreachable if SEC_MAPPING is properly defined (which should be
    // the case if the user has no warning).
    return std::nullopt;

    #undef SEC_EQUIV
}

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::toExternalOpt(Internal internal) -> std::optional<External>
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) case INT_VAL: return EXT_VAL;

    switch (internal) {
        SEC_MAPPING
    }

    // This is unreachable if SEC_MAPPING is properly defined (which should be
    // the case if the user has no warning).
    return std::nullopt;

    #undef SEC_EQUIV
}

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::internalValues() -> const std::set<Internal>&
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) INT_VAL,

    static const std::set<Internal> values {
        SEC_MAPPING
    };

    return values;

    #undef SEC_EQUIV
}

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::externalValues() -> const std::set<External>&
{
    #define SEC_EQUIV(INT_VAL, EXT_VAL) EXT_VAL,

    static const std::set<External> values {
        SEC_MAPPING
    };

    return values;

    #undef SEC_EQUIV
}

#undef SEC_INTERNAL
#undef SEC_EXTERNAL
#undef SEC_MAPPING
