pub trait PartialEnumConverter<FromType, ToType> {
    fn convert_opt(from_value: &FromType) -> Option<ToType>;
    fn convertible_values() -> &'static [FromType];
}

pub unsafe trait EnumConverter<FromType, ToType>:
    PartialEnumConverter<FromType, ToType>
{
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

    #[derive(Debug, Eq, PartialEq)]
    enum C {
        C1,
        C2,
        C3,
    }

    bidi!(* DirectConverter<A, B> {
        A1 == B1,
        A2 == B2,
    });

    bidi!(* ReversedConverter<A, B> {
        A1 == B2,
        A2 == B1,
    });

    bidi!(* SemiCompleteConverter<A,C> {
        A1 == C1,
        A2 == C2,
        _  <= C3
    });

    bidi!(* ProjectedConverter<A,C> {
        A1 == C1,
        A2 == C2,
        A1 <= C3
    });

    #[test]
    fn direct_convertible_values() {
        assert_eq!(
            <DirectConverter as PartialEnumConverter<A, B>>::convertible_values(),
            [A::A1, A::A2]
        );
        assert_eq!(
            <DirectConverter as PartialEnumConverter<B, A>>::convertible_values(),
            [B::B1, B::B2]
        );
    }

    #[test]
    fn reversed_convertible_values() {
        assert_eq!(
            <ReversedConverter as PartialEnumConverter<A, B>>::convertible_values(),
            [A::A1, A::A2]
        );
        assert_eq!(
            <ReversedConverter as PartialEnumConverter<B, A>>::convertible_values(),
            [B::B2, B::B1]
        );
    }

    #[test]
    fn semi_complete_convertible_values() {
        assert_eq!(
            <SemiCompleteConverter as PartialEnumConverter<A, C>>::convertible_values(),
            [A::A1, A::A2]
        );
        assert_eq!(
            <SemiCompleteConverter as PartialEnumConverter<C, A>>::convertible_values(),
            [C::C1, C::C2]
        );
    }

    #[test]
    fn projected_convertible_values() {
        assert_eq!(
            <ProjectedConverter as PartialEnumConverter<A, C>>::convertible_values(),
            [A::A1, A::A2]
        );
        assert_eq!(
            <ProjectedConverter as PartialEnumConverter<C, A>>::convertible_values(),
            [C::C1, C::C2, C::C3]
        );
    }

    #[test]
    fn direct_opt_conversions() {
        // First direction
        assert_eq!(DirectConverter::convert_opt(&A::A1), Some(B::B1));
        assert_eq!(DirectConverter::convert_opt(&A::A2), Some(B::B2));
        // Second direction
        assert_eq!(DirectConverter::convert_opt(&B::B1), Some(A::A1));
        assert_eq!(DirectConverter::convert_opt(&B::B2), Some(A::A2));
    }

    #[test]
    fn direct_conversions() {
        // First direction
        assert_eq!(DirectConverter::convert(&A::A1), B::B1);
        assert_eq!(DirectConverter::convert(&A::A2), B::B2);
        // Second direction
        assert_eq!(DirectConverter::convert(&B::B1), A::A1);
        assert_eq!(DirectConverter::convert(&B::B2), A::A2);
    }

    #[test]
    fn reversed_opt_conversions() {
        // First direction
        assert_eq!(ReversedConverter::convert_opt(&A::A1), Some(B::B2));
        assert_eq!(ReversedConverter::convert_opt(&A::A2), Some(B::B1));
        // Second direction
        assert_eq!(ReversedConverter::convert_opt(&B::B2), Some(A::A1));
        assert_eq!(ReversedConverter::convert_opt(&B::B1), Some(A::A2));
    }

    #[test]
    fn reversed_conversions() {
        // First direction
        assert_eq!(ReversedConverter::convert(&A::A1), B::B2);
        assert_eq!(ReversedConverter::convert(&A::A2), B::B1);
        // Second direction
        assert_eq!(ReversedConverter::convert(&B::B2), A::A1);
        assert_eq!(ReversedConverter::convert(&B::B1), A::A2);
    }

    #[test]
    fn semi_complete_opt_conversions() {
        // First direction
        assert_eq!(SemiCompleteConverter::convert_opt(&A::A1), Some(C::C1));
        assert_eq!(SemiCompleteConverter::convert_opt(&A::A2), Some(C::C2));
        // Second direction
        assert_eq!(SemiCompleteConverter::convert_opt(&C::C1), Some(A::A1));
        assert_eq!(SemiCompleteConverter::convert_opt(&C::C2), Some(A::A2));
        assert_eq!(SemiCompleteConverter::convert_opt(&C::C3), None);
    }

    #[test]
    fn projected_opt_conversions() {
        // First direction
        assert_eq!(ProjectedConverter::convert_opt(&A::A1), Some(C::C1));
        assert_eq!(ProjectedConverter::convert_opt(&A::A2), Some(C::C2));
        // Second direction
        assert_eq!(ProjectedConverter::convert_opt(&C::C1), Some(A::A1));
        assert_eq!(ProjectedConverter::convert_opt(&C::C2), Some(A::A2));
        assert_eq!(ProjectedConverter::convert_opt(&C::C3), Some(A::A1));
    }

    #[test]
    fn projected_conversions() {
        // First direction
        assert_eq!(ProjectedConverter::convert(&A::A1), C::C1);
        assert_eq!(ProjectedConverter::convert(&A::A2), C::C2);
        // Second direction
        assert_eq!(ProjectedConverter::convert(&C::C1), A::A1);
        assert_eq!(ProjectedConverter::convert(&C::C2), A::A2);
        assert_eq!(ProjectedConverter::convert(&C::C3), A::A1);
    }
}
