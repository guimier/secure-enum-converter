#ifndef LGUIM_SECURE_ENUM_CONVERTER
#define LGUIM_SECURE_ENUM_CONVERTER

#include <set>
#include <stdexcept>
#include <type_traits>

#ifndef SEC_OPTIONAL_NS
#define SEC_OPTIONAL_NS std
#include <optional>
#endif

namespace lguim {

/** `SecureEnumConverter` is a bi-directional enum converter, which
 * needs only one mapping in code to do both directions, and will
 * only compile when the behavior for all values is explicitely
 * written in code.
 *
 * @tparam InternalType One of the two enumeration types.
 * @tparam ExternalType The other enumeration type.
 * @tparam Tag This optional type can be defined to any custom type
 *     when you need to have two different mappings between the same
 *     types. There is no need for the type to be instanciable, only
 *     to have an identity.
 *     Exemple:
 *     ````
 *     enum class A { A1, A2 };
 *     enum class B { B1, B2 };
 *     using StandardConverter = SecureEnumConverter<A, B, struct StandardConverterTag>;
 *     using AlternateConverter = SecureEnumConverter<A, B, struct AlternateConverterTag>;
 *     ```
 *
 * The internal/external naming is used everywhere in the interface
 * to differenciate the direction. Finding a good naming being at
 * the same time generic, easy to use and easy to read is hard, and
 * we believe the internal/external distinction is the one that will
 * be most helpful where this class is needed. We apologize if this
 * is not the case for you.
 *
 * The implementation of the methods should sit in its own translation
 * unit.
 *
 * To give this class the correct mapping to use, you need to define
 * two macros and include a file:
 *
 * ```
 * enum class A { A1, A2 };
 * enum class B { B1, B2 };
 * using Converter = lguim::SecureEnumConverter<A, B>;
 *
 * #define SEC_TYPE Converter
 * #define SEC_MAPPING \
 *     SEC_EQUIV(A::A1, B::B1) \
 *     SEC_EQUIV(A::A2, B::B2)
 * #include "lguim/secureenumconverter.inc"
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
template <typename InternalType, typename ExternalType, typename Tag = void>
struct SecureEnumConverter {
    using Internal = InternalType;
    using External = ExternalType;
    using Converter = SecureEnumConverter<Internal, External, Tag>;

    static SEC_OPTIONAL_NS::optional<Internal> toInternalOpt(External);
    static SEC_OPTIONAL_NS::optional<External> toExternalOpt(Internal);

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

namespace priv {
    template <bool toExternal, bool toInternal, typename Converter>
    struct OneDirectionConverter;

    template <typename Converter> // To external
    struct OneDirectionConverter<true, false, Converter> {
        using Input = typename Converter::Internal;
        using Output = typename Converter::External;

        static SEC_OPTIONAL_NS::optional<Output> convertOpt(Input input)
        { return Converter::toExternalOpt(input); }

        static Output convertOrThrow(Input input)
        { return Converter::toExternalOrThrow(input); }

        static const std::set<Output>& convertibleValues()
        { return Converter::convertibleExternalValues(); }
    };

    template <typename Converter> // To internal
    struct OneDirectionConverter<false, true, Converter> {
        using Input = typename Converter::External;
        using Output = typename Converter::Internal;

        static SEC_OPTIONAL_NS::optional<Output> convertOpt(Input input)
        { return Converter::toInternalOpt(input); }

        static Output convertOrThrow(Input input)
        { return Converter::toInternalOrThrow(input); }

        static const std::set<Output>& convertibleValues()
        { return Converter::convertibleInternalValues(); }
    };
};

template <typename InternalTag, typename InternalType, typename ExternalTag, typename ExternalType, typename Tag = void>
struct TaggedEnumConverter: public SecureEnumConverter<InternalType, ExternalType, Tag> {

    template <typename DirectionTag, typename ODC = priv::OneDirectionConverter<
        std::is_same<DirectionTag, ExternalTag>::value,
        std::is_same<DirectionTag, InternalTag>::value,
        SecureEnumConverter<InternalType, ExternalType, Tag>
    > >
    static SEC_OPTIONAL_NS::optional<typename ODC::Output> convertOpt(typename ODC::Input input) {
        return ODC::convertOpt(input);
    }

    template <typename DirectionTag, typename ODC = priv::OneDirectionConverter<
        std::is_same<DirectionTag, ExternalTag>::value,
        std::is_same<DirectionTag, InternalTag>::value,
        SecureEnumConverter<InternalType, ExternalType, Tag>
    > >
    static typename ODC::Output convertOrThrow(typename ODC::Input input) {
        return ODC::convertOrThrow(input);
    }

    template <typename DirectionTag, typename ODC = priv::OneDirectionConverter<
        std::is_same<DirectionTag, ExternalTag>::value,
        std::is_same<DirectionTag, InternalTag>::value,
        SecureEnumConverter<InternalType, ExternalType, Tag>
    > >
    static std::set<typename ODC::Output> convertibleValues() {
        return ODC::convertibleValues();
    }

};

} // namespace lguim

#endif // LGUIM_SECURE_ENUM_CONVERTER
