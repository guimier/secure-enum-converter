extern crate proc_macro;

use std::iter::FromIterator;
use proc_macro2::{TokenStream, Delimiter, TokenTree, Spacing};
use quote::{quote, format_ident};

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

    pub type Rules = Vec<Rule>;

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

    type Data = model::Rules;

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

fn mirror(direct: &model::Definition) -> model::Definition {
    use model::Rule::*;

    model::Definition {
        name: direct.name.clone(),
        external_type: direct.internal_type.clone(),
        internal_type: direct.external_type.clone(),
        rules: direct.rules.iter()
            .map(|rule| match rule.clone() {
                Equivalence(internal, external) => Equivalence(external, internal),
                ProjectionToExternal(internal, external) => ProjectionToInternal(external, internal),
                ProjectionToInternal(internal, external) => ProjectionToExternal(external, internal),
                OrphanExternal(external) => OrphanInternal(external),
                OrphanInternal(internal) => OrphanExternal(internal),
            })
            .collect()
    }
}

const INTERNAL_CONVERTIBLES_CONSTANT: &str = "INTERNAL_CONVERTIBLES";
const EXTERNAL_CONVERTIBLES_CONSTANT: &str = "EXTERNAL_CONVERTIBLES";

fn generate_convertibles_array(type_: &TokenStream, rules: &model::Rules) -> TokenStream {
    let names = rules.iter()
        .filter_map(|rule| match rule {
            model::Rule::Equivalence(internal, ..) => Some(internal),
            model::Rule::ProjectionToInternal(..) => None,
            model::Rule::ProjectionToExternal(internal, ..) => Some(internal),
            model::Rule::OrphanInternal(..) => None,
            model::Rule::OrphanExternal(..) => None,
        })
        .map(|variant| quote!(#type_::#variant));

    quote!{
        [
            #(#names ,)*
        ]
    }
}

fn generate_struct_impl(def: &model::Definition, mirror_rules: &model::Rules) -> TokenStream {
    let model::Definition { name, internal_type, external_type, rules: direct_rules } = def;
    let int_constant_name = format_ident!("{}", INTERNAL_CONVERTIBLES_CONSTANT);
    let ext_constant_name = format_ident!("{}", EXTERNAL_CONVERTIBLES_CONSTANT);
    let int_constant = generate_convertibles_array(internal_type, direct_rules);
    let ext_constant = generate_convertibles_array(external_type, mirror_rules);

    quote!{
        impl #name {
            const #int_constant_name: &'static [#internal_type] = &#int_constant;
            const #ext_constant_name: &'static [#external_type] = &#ext_constant;
        }
    }
}

fn generate_partial_impl(def: &model::Definition, const_name: &str) -> TokenStream {
    let model::Definition { name, internal_type, external_type, rules } = def;
    let constant_name = format_ident!("{}", const_name);

    // TODO Refer to ::bidi:: instead of crate::
    quote!{
        impl crate::PartialEnumConverter<#internal_type, #external_type> for #name {
            fn convert_opt(internal_value: &#internal_type) -> Option<#external_type> {
                None
            }

            fn convertible_values() -> &'static [#internal_type] {
                Self::#constant_name
            }
        }
    }
}

#[proc_macro]
pub fn bidi(item: proc_macro::TokenStream) -> proc_macro::TokenStream {
    let item = TokenStream::from(item);

    let definition = type_parser::parse(item);
    let mirror = mirror(&definition);

    let name = &definition.name;
    let out_definition = quote!( struct #name {} );

    let out_struct_impl = generate_struct_impl(&definition, &mirror.rules);
    let out_partial_impl1 = generate_partial_impl(&definition, INTERNAL_CONVERTIBLES_CONSTANT);
    let out_partial_impl2 = generate_partial_impl(&mirror, EXTERNAL_CONVERTIBLES_CONSTANT);

    proc_macro::TokenStream::from(quote! {
        #out_definition
        #out_struct_impl
        #out_partial_impl1
        #out_partial_impl2
    })
}
