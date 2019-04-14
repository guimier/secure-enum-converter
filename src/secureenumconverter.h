#ifndef SECURE_ENUM_CONVERTER
#define SECURE_ENUM_CONVERTER

#include <optional>
#include <vector>
#include <stdexcept>

template <typename InternalType, typename ExternalType>
struct SecureEnumConverter {
  using Internal = InternalType;
  using External = ExternalType;

  static std::optional<Internal> toInternalOpt(External);
  static std::optional<External> toExternalOpt(Internal);

  static const std::vector<Internal>& internalValues();
  static const std::vector<External>& externalValues();

  static Internal toInternalOrThrow(External external) {
    const auto& internalOpt = toInternalOpt(external);

    if (internalOpt) {
      throw std::invalid_argument("Invalid external enum value");
    }

    return *internalOpt;
  }

  static External toExternalOrThrow(Internal internal) {
    const auto& externalOpt = toExternalOpt(internal);

    if (externalOpt) {
      throw std::invalid_argument("Invalid internal enum value");
    }

    return *externalOpt;
  }

};

#endif // SECURE_ENUM_CONVERTER
