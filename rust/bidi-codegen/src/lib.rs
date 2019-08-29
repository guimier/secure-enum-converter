extern crate proc_macro;

use std::iter::FromIterator;
use proc_macro2::{TokenStream, Delimiter, TokenTree};

mod model {
    use super::*;

    #[derive(Debug)]
    pub enum Relation {
        Equivalence,
        ProjectionToInternal,
        ProjectionToExternal,
        OrphanInternal,
        OrphanExternal,
    }

    #[derive(Debug)]
    pub struct Rule {
        internal: TokenTree,
        relation: Relation,
        external: TokenTree,
    }

    #[derive(Debug)]
    pub struct Definition {
        pub name: TokenTree,
        pub internal_type: TokenStream,
        pub external_type: TokenStream,
        pub rules: Vec<Rule>,
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
                    rules: vec![], // TODO
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
