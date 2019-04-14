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
    // TODO
}

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::toExternalOpt(Internal internal) -> std::optional<External>
{
    // TODO
}
  static const std::vector<InternalEnum>& internalValues();
  static const std::vector<ExternalEnum>& externalValues();

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::internalValues() -> std::vector<Internal>
{
    // TODO
}

template <>
auto SecureEnumConverter<SEC_INTERNAL, SEC_EXTERNAL>::externalValues() -> std::vector<External>
{
    // TODO
}

#undef SEC_INTERNAL
#undef SEC_EXTERNAL
#undef SEC_MAPPING
