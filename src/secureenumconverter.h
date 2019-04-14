#ifndef SECURE_ENUM_CONVERTER
#define SECURE_ENUM_CONVERTER

#include <optional>
#include <set>
#include <stdexcept>

/** `SecureEnumConverter` is a bi-directional enum converter, which
 * needs only one mapping in code to do both directions, and will
 * only compile when the behavior for all values is explicitely
 * written in code.
 *
 * @tparam InternalType One on the two enumeration types.
 * @tparam ExternalType The other enumeration type.
 *
 * The internal/external naming is used everywhere in the interface
 * to differenciate the direction. Finding a good naming being at
 * the same time generic, easy to use and easy to read is hard, and
 * we believe the internal/external distinction is the one that will
 * be most helpful where this class is needed. We apologize if this
 * is not the case for you.
 *
 * The implementation of the methods can either be inlined, or sit
 * in its own translation unit. The latter is probably better and is
 * the only one that is really tested.
 *
 * To give this class the correct mapping to use, you need to define
 * two macros and include a file:
 *
 * ```
 * enum class A { A1, A2 };
 * enum class B { B1, B2 };
 * using Converter = SecureEnumConverter<A, B>;
 *
 * #define SEC_TYPE Converter
 * #define SEC_MAPPING \
 *     SEC_EQUIV(A::A1, B::B1) \
 *     SEC_EQUIV(A::A2, B::B2)
 * #include "secureenumconverter.inc"
 * ```
 *
 * This inclusion must happen outside any namespace. It can happen
 * multiple times in the same translation unit.
 *
 * The following macros are available for the mapping:
 *
 *   - `SEC_EQUIV` describes a direct equivalence between two values
 *     SEC_EQUIV(x_int, x_ext) ⇔ { to_ext(x_int) = x_ext; to_int(x_ext) = x_int }
 *   - `SEC_PROJ_I2E` describes a unidirectional conversion from internal to external
 *     SEC_PROJ_I2E(x_int, x_ext) ⇔ { to_ext(x_int) = x_ext; to_int(x_ext) is defined separately }
 *   - `SEC_PROJ_E2I` describes a unidirectional conversion from external to internal
 *     SEC_PROJ_E2I(x_int, x_ext) ⇔ { to_int(x_ext) = x_int; to_ext(x_int) is defined separately }
 *   - `SEC_ORPHAN_INT` marks an internal value as having no matching external value
 *     SEC_ORPHAN_INT(x_int) ⇔ { to_ext(x_int) is undefined }
 *   - `SEC_ORPHAN_EXT` marks an internal value as having no matching external value
 *     SEC_ORPHAN_EXT(x_ext) ⇔ { to_int(x_ext) is undefined }
 */
template <typename InternalType, typename ExternalType>
struct SecureEnumConverter {
    using Internal = InternalType;
    using External = ExternalType;

    static std::optional<Internal> toInternalOpt(External);
    static std::optional<External> toExternalOpt(Internal);

    static const std::set<Internal>& convertibleInternalValues();
    static const std::set<External>& convertibleExternalValues();

    static Internal toInternalOrThrow(External external) {
        const auto& internalOpt = toInternalOpt(external);

        if (!internalOpt) {
          throw std::invalid_argument("Invalid external enum value");
        }

        return *internalOpt;
    }

    static External toExternalOrThrow(Internal internal) {
        const auto& externalOpt = toExternalOpt(internal);

        if (!externalOpt) {
          throw std::invalid_argument("Invalid internal enum value");
        }

        return *externalOpt;
    }

};

#endif // SECURE_ENUM_CONVERTER
