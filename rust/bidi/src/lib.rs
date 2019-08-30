pub trait PartialEnumConverter<FromType, ToType> {
    fn convert_opt(from_value: &FromType) -> Option<ToType>;
    fn convertible_values() -> &'static [FromType];
}

pub trait EnumConverter<FromType, ToType>: PartialEnumConverter<FromType, ToType> {
    fn convert(from_value: &FromType) -> ToType {
        Self::convert_opt(from_value).unwrap()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use bidi_codegen::*;

    #[derive(Debug, Eq, PartialEq)]
    enum A {
        A1,
        A2,
    }
    #[derive(Debug, Eq, PartialEq)]
    enum B {
        B1,
        B2,
    }

    /* Rule syntax idea goal:
     *   - equivalence: `A1 == B1`
     *   - projection:  `A3 => B1`, `A2 <= B4`
     *   - orphan:      `A5 => _ `, `_  <= B6`
     */

    bidi!(DirectConverter<A, B> {
        A1 == B1,
        A2 == B2,
    });

    bidi!(ReversedConverter<A, B> {
        A1 == B2,
        A2 == B1,
    });

    #[test]
    fn direct_convertible_values() {
        assert_eq!(
            <DirectConverter as PartialEnumConverter<A, B>>::convertible_values(),
            [A::A1, A::A2]
        );
    }

    #[test]
    fn reversed_convertible_values() {
        assert_eq!(
            <DirectConverter as PartialEnumConverter<B, A>>::convertible_values(),
            [B::B1, B::B2]
        );
    }

    #[test]
    fn direct_to_external() {
        assert_eq!(DirectConverter::convert_opt(&A::A1), Some(B::B1));
        assert_eq!(DirectConverter::convert_opt(&A::A2), Some(B::B2));
    }

    #[test]
    fn reversed_to_external() {
        assert_eq!(DirectConverter::convert_opt(&A::A1), Some(B::B2));
        assert_eq!(DirectConverter::convert_opt(&A::A2), Some(B::B1));
    }
}
