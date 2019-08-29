extern crate proc_macro;

use std::iter::FromIterator;
use proc_macro2::{TokenStream, Delimiter, TokenTree, Spacing};

mod model {
    use super::*;

    #[derive(Debug, Clone)]
    pub enum Rule {
        Equivalence(TokenTree, TokenTree),
        ProjectionToInternal(TokenTree, TokenTree),
        ProjectionToExternal(TokenTree, TokenTree),
        OrphanInternal(TokenTree),
        OrphanExternal(TokenTree),
    }

    #[derive(Debug)]
    pub struct Definition {
        pub name: TokenTree,
        pub internal_type: TokenStream,
        pub external_type: TokenStream,
        pub rules: Vec<Rule>,
    }
}

mod rules_parser {
    use super::*;

    type Data = Vec<model::Rule>;

    #[derive(Debug, Clone, Copy)] enum Op1 { Eq, Lt }
    #[derive(Debug, Clone, Copy)] enum Op2 { Eq, Gt }

    #[derive(Debug)]
    enum State {
        StartOfRule(Data),
        Internal(Data, TokenTree),
        FirstOperatorChar(Data, TokenTree, Op1),
        SecondOperatorChar(Data, TokenTree, Op1, Op2),
        EndOfRule(Data),
    }

    fn is_trap(token: &TokenTree) -> bool {
        if let TokenTree::Ident(i) = token {
            i.to_string() == "_"
        } else {
            false
        }
    }

    fn next_step(state: State, token: TokenTree) -> State {
        use State::*;
        use TokenTree::*;

        match (state, token.clone()) {
            (StartOfRule(data), Ident(..)) =>
                Internal(data, token),

            (Internal(ref data, ref internal), Punct(ref p)) if p.spacing() == Spacing::Joint && p.as_char() == '=' =>
                FirstOperatorChar(data.clone(), internal.clone(), Op1::Eq),

            (Internal(ref data, ref internal), Punct(ref p)) if p.spacing() == Spacing::Joint && p.as_char() == '<' =>
                FirstOperatorChar(data.clone(), internal.clone(), Op1::Lt),

            (FirstOperatorChar(ref data, ref internal, op1), Punct(ref p)) if p.as_char() == '=' =>
                SecondOperatorChar(data.clone(), internal.clone(), op1, Op2::Eq),

            (FirstOperatorChar(ref data, ref internal, op1), Punct(ref p)) if p.as_char() == '>' =>
                SecondOperatorChar(data.clone(), internal.clone(), op1, Op2::Gt),

            (SecondOperatorChar(ref data, ref internal, op1, op2), Ident(..)) => {
                let internal = internal.clone();
                let external = token;
                let mut data = data.clone();

                data.push(match (op1, op2) {
                    (Op1::Eq, Op2::Eq) => {
                        assert!(!is_trap(&internal));
                        assert!(!is_trap(&external));
                        model::Rule::Equivalence(internal, external)
                    },
                    (Op1::Eq, Op2::Gt) => {
                        assert!(!is_trap(&internal));
                        if is_trap(&external) {
                            model::Rule::ProjectionToExternal(internal, external)
                        } else {
                            model::Rule::OrphanInternal(internal)
                        }
                    },
                    (Op1::Lt, Op2::Eq) => {
                        assert!(!is_trap(&external));
                        if is_trap(&internal) {
                            model::Rule::ProjectionToInternal(internal, external)
                        } else {
                            model::Rule::OrphanExternal(external)
                        }
                    },
                    (Op1::Lt, Op2::Gt) => {
                        panic!("<> is not an acceptable relation")
                    }
                });

                EndOfRule(data)
            },

            (EndOfRule(ref data), Punct(ref p)) if p.as_char() == ',' =>
                StartOfRule(data.clone()),

            (st, to) => unimplemented!("{:?} {:?}", st, to),
        }
    }

    pub fn parse(stream: TokenStream) -> Vec<model::Rule> {
        let mut state = State::StartOfRule(vec![]);

        for token in stream.into_iter() {
            state = next_step(state, token);
        }

        match state {
            State::StartOfRule(data) | State::EndOfRule(data) => data,
            _ => panic!("Reached end of stream unexpectedly"),
        }
    }
}

mod type_parser {
    use super::*;

    #[derive(Debug, Clone)]
    struct Data {
        name: TokenTree,
        internal: Vec<TokenTree>,
        external: Vec<TokenTree>
    }

    impl Data {
        pub fn new(name: TokenTree) -> Self {
            Data {
                name,
                internal: vec![],
                external: vec![],
            }
        }

        pub fn add_internal(&self, token: TokenTree) -> Self {
            let mut internal = self.internal.clone();
            internal.push(token);
            Data {
                name: self.name.clone(),
                internal,
                external: self.external.clone(),
            }
        }

        pub fn add_external(&self, token: TokenTree) -> Self {
            let mut external = self.external.clone();
            external.push(token);
            Data {
                name: self.name.clone(),
                internal: self.internal.clone(),
                external,
            }
        }
    }

    #[derive(Debug)]
    enum State {
        Init,
        Named(Data),
        WillReadInternal(Data),
        InternalReady(Data),
        InternalSub1(Data), // TODO
        InternalSub2(Data), // TODO
        WillReadExternal(Data),
        ExternalReady(Data),
        ExternalSub1(Data), // TODO
        ExternalSub2(Data), // TODO
        Specified(Data),
        Done(model::Definition),
    }

    fn next_step(state: State, token: TokenTree) -> State {
        use State::*;
        use TokenTree::*;

        match (state, token.clone()) {
            (Init, Ident(..)) =>
                Named(Data::new(token)),

            (Named(ref data), Punct(ref p)) if p.as_char() == '<' =>
                WillReadInternal(data.clone()),

            (WillReadInternal(data), Ident(..)) =>
                InternalReady(data.add_internal(token)),

            (InternalReady(ref data), Punct(ref p)) if p.as_char() == ',' =>
                WillReadExternal(data.clone()),

            (WillReadExternal(data), Ident(..)) =>
                ExternalReady(data.add_external(token)),

            (ExternalReady(ref data), Punct(ref p)) if p.as_char() == '>' =>
                Specified(data.clone()),

            (Specified(ref data), Group(ref g)) if g.delimiter() == Delimiter::Brace =>
                Done(model::Definition {
                    name: data.name.clone(),
                    internal_type: TokenStream::from_iter(data.internal.clone()),
                    external_type: TokenStream::from_iter(data.external.clone()),
                    rules: rules_parser::parse(g.stream()),
                }),

            (st, to) => unimplemented!("{:?} {:?}", st, to),
        }
    }

    pub fn parse(stream: TokenStream) -> model::Definition {
        let mut state = State::Init;

        for token in stream.into_iter() {
            state = next_step(state, token);
        }

        if let State::Done(definition) = state {
            definition
        } else {
            panic!("Reached end of stream unexpectedly")
        }
    }
}

#[proc_macro]
pub fn bidi(item: proc_macro::TokenStream) -> proc_macro::TokenStream {
    let item = TokenStream::from(item);
    let definition = type_parser::parse(item);
    let name = definition.name;

    proc_macro::TokenStream::from(quote::quote!(
        struct #name {}
    ))
}
