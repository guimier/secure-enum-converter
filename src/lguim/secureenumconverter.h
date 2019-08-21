// Copyright 2019 Lucien Guimier <lucien.guimier@laposte.net>
// Released according to the MIT terms. See attached LICENSE file.

#ifndef LGUIM_SECUREENUMCONVERTER_H_
#define LGUIM_SECUREENUMCONVERTER_H_

#include <set>
#include <stdexcept>
#include <type_traits>
#include <sstream>

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
 * be most helpful where this class is needed. However, this class
 * supports subclassing (including giving the subclass as `SEC_TYPE`).
 * You can use the `TaggedEnumConverter` if it is a better fit for your
 * usage, or define your own subclass.
 *
 * To give this class the correct mapping to use, you need to define
 * two macros and include a file. The implementation file must be
 * included in exactly one translation unit. Example:
 *
 * ```
 * // abconverter.h
 * #include "lguim/secureenumconverter.h"
 * enum class A { A1, A2 };
 * enum class B { B1, B2 };
 * using Converter = lguim::SecureEnumConverter<A, B>;
 *
 * // abconverter.cpp
 * #include "abconverter.h"
 *
 * #define SEC_TYPE Converter
 * #define SEC_MAPPING \
 *     SEC_EQUIV(A::A1, B::B1) \
 *     SEC_EQUIV(A::A2, B::B2)
 * #include "lguim/secureenumconverter.inc"
 * ```
 *
 * This inclusion must happen outside any namespace. The file can be
 * included multiple times in the same translation unit, as long as
 * there is only one inclusion for each type in the project.
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
            std::ostringstream oss;
            oss << "Invalid external enum value (" << converter() << ")";
            throw std::invalid_argument(oss.str());
        }

        return *internalOpt;
    }

    static External toExternalOrThrow(Internal internal) {
        const auto& externalOpt = toExternalOpt(internal);

        if (!externalOpt) {
            std::ostringstream oss;
            oss << "Invalid external enum value (" << converter() << ")";
            throw std::invalid_argument(oss.str());
        }

        return *externalOpt;
    }

 private:
    static const char* converter() { return __PRETTY_FUNCTION__; }
};

namespace priv {

template <bool toExternal, typename Converter>
struct OneDirectionConverter;

template <typename Converter>  // To external
struct OneDirectionConverter<true, Converter> {
    using Input = typename Converter::Internal;
    using Output = typename Converter::External;

    static SEC_OPTIONAL_NS::optional<Output> convertOpt(Input input)
    { return Converter::toExternalOpt(input); }

    static Output convertOrThrow(Input input)
    { return Converter::toExternalOrThrow(input); }

    static const std::set<Output>& convertibleValues()
    { return Converter::convertibleExternalValues(); }
};

template <typename Converter>  // To internal
struct OneDirectionConverter<false, Converter> {
    using Input = typename Converter::External;
    using Output = typename Converter::Internal;

    static SEC_OPTIONAL_NS::optional<Output> convertOpt(Input input)
    { return Converter::toInternalOpt(input); }

    static Output convertOrThrow(Input input)
    { return Converter::toInternalOrThrow(input); }

    static const std::set<Output>& convertibleValues()
    { return Converter::convertibleInternalValues(); }
};

}  // namespace priv

/** `TaggedEnumConverter` is a subclass of `SecureEnumConverter`, providing a
 * more natural interface to the `SecureEnumConverter` API than the primary
 * one centered around the internal/external terminology.
 *
 * It may be used in a very similar way:
 * ```
 * enum class A { A1, A2 }; struct TA;
 * enum class B { B1, B2 }; struct TB;
 * using Converter = lguim::TaggedEnumConverter<TA, A, TB, B>;
 *
 * #define SEC_TYPE Converter
 * #define SEC_MAPPING \
 *     SEC_EQUIV(A::A1, B::B1) \
 *     SEC_EQUIV(A::A2, B::B2)
 * #include "lguim/secureenumconverter.inc"
 * ```
 *
 * Except you now call `convertOpt<TA>` instead of `toInternalOpt`.
 *
 * This class implements a interface that may still be too generic for your
 * needs, because it allows using a tag type that is not one of the enumeration
 * types. This may be useful when you have two families of enumerations to
 * convert and want the tag to describe the family rather than the specific type.
 *
 * If you don’t need a different type, you may use `TypedEnumConverter` instead.
 */
template <
    typename InternalTag, typename InternalType,
    typename ExternalTag, typename ExternalType,
    typename Tag = void
>
struct TaggedEnumConverter:
    public SecureEnumConverter<InternalType, ExternalType, Tag> {
    /** Helper for implementation of tag-using method.
     *
     * This type is exposed in case you want to add extension methods or
     * free functions using the same “tag” mechanism.
     */
    template <typename DirectionTag>
    using HalfConverter = priv::OneDirectionConverter<
        std::is_same<DirectionTag, ExternalTag>::value,
        SecureEnumConverter<InternalType, ExternalType, Tag>
    >;

    template <typename DirectionTag>
    using Output = typename HalfConverter<DirectionTag>::Output;

    template <typename DirectionTag>
    using Input = typename HalfConverter<DirectionTag>::Input;

    template <typename DirectionTag>
    static SEC_OPTIONAL_NS::optional<Output<DirectionTag>>
    convertOpt(Input<DirectionTag> input) {
        return HalfConverter<DirectionTag>::convertOpt(input);
    }

    template <typename DirectionTag>
    static Output<DirectionTag>
    convertOrThrow(Input<DirectionTag> input) {
        return HalfConverter<DirectionTag>::convertOrThrow(input);
    }

    template <typename DirectionTag>
    static std::set<Output<DirectionTag>>
    convertibleValues() {
        return HalfConverter<DirectionTag>::convertibleValues();
    }
};

/** `TaggedEnumConverter` is a subclass of `SecureEnumConverter`, providing a
 * more natural interface to the `SecureEnumConverter` API than the primary
 * one centered around the internal/external terminology.
 *
 * It may be used in a very similar way:
 * ```
 * enum class A { A1, A2 };
 * enum class B { B1, B2 };
 * using Converter = lguim::TypedEnumConverter<A, B>;
 *
 * #define SEC_TYPE Converter
 * #define SEC_MAPPING \
 *     SEC_EQUIV(A::A1, B::B1) \
 *     SEC_EQUIV(A::A2, B::B2)
 * #include "lguim/secureenumconverter.inc"
 * ```
 *
 * Except you now call `convertOpt<A>` instead of `toInternalOpt`.
 */
template <typename InternalType, typename ExternalType, typename Tag = void>
using TypedEnumConverter = TaggedEnumConverter<
    InternalType, InternalType,
    ExternalType, ExternalType,
    Tag
>;

}  // namespace lguim

#endif  // LGUIM_SECUREENUMCONVERTER_H_
