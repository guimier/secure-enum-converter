extern crate proc_macro;

use proc_macro2::{Delimiter, Spacing, TokenStream, TokenTree};
use quote::{format_ident, quote};
use std::iter::FromIterator;

struct Error {
    message: String,
    // TODO span: Span,
}

impl Error {
    pub fn new(msg: &str) -> Self {
        Error {
            message: msg.into()
        }
    }
}

type Result<T> = std::result::Result<T, Error>;

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
        pub for_tests: bool,
        pub name: TokenTree,
        pub internal_type: TokenStream,
        pub external_type: TokenStream,
        pub rules: Vec<Rule>,
    }
}

mod rules_parser {
    use super::*;

    type Data = model::Rules;

    #[derive(Debug, Clone, Copy)]
    enum Op {
        Eq,
        Gt,
        Lt,
    }

    #[derive(Debug)]
    enum State {
        StartOfRule(Data),
        Internal(Data, TokenTree),
        Operator(Data, TokenTree, Op),
        EndOfRule(Data),
    }

    fn is_trap(token: &TokenTree) -> bool {
        if let TokenTree::Ident(i) = token {
            i.to_string() == "_"
        } else {
            false
        }
    }

    fn next_step(state: State, token: TokenTree) -> Result<State> {
        use State::*;
        use TokenTree::*;

        Ok(match (state, token.clone()) {
            (StartOfRule(data), Ident(..)) => Internal(data, token),

            (Internal(ref data, ref internal), Punct(ref p)) if p.as_char() == '=' =>
                Operator(data.clone(), internal.clone(), Op::Eq),

            (Internal(ref data, ref internal), Punct(ref p)) if p.as_char() == '<' =>
                Operator(data.clone(), internal.clone(), Op::Lt),

            (Internal(ref data, ref internal), Punct(ref p)) if p.as_char() == '>' =>
                Operator(data.clone(), internal.clone(), Op::Gt),

            (Operator(ref data, ref internal, op), Ident(..)) => {
                let internal = internal.clone();
                let external = token;
                let mut data = data.clone();

                data.push(match op {
                    Op::Eq => {
                        assert!(!is_trap(&internal));
                        assert!(!is_trap(&external));
                        model::Rule::Equivalence(internal, external)
                    }
                    Op::Gt => {
                        assert!(!is_trap(&internal));
                        if is_trap(&external) {
                            model::Rule::OrphanInternal(internal)
                        } else {
                            model::Rule::ProjectionToExternal(internal, external)
                        }
                    }
                    Op::Lt => {
                        assert!(!is_trap(&external));
                        if is_trap(&internal) {
                            model::Rule::OrphanExternal(external)
                        } else {
                            model::Rule::ProjectionToInternal(internal, external)
                        }
                    }
                });

                EndOfRule(data)
            }

            (EndOfRule(ref data), Punct(ref p)) if p.as_char() == ',' => StartOfRule(data.clone()),

            (_, to) => return Err(Error::new(&format!("Unexpected token {}", to)))
        })
    }

    pub(crate) fn parse(stream: TokenStream) -> Result<Vec<model::Rule>> {
        let mut state = State::StartOfRule(vec![]);

        for token in stream.into_iter() {
            state = next_step(state, token)?;
        }

        match state {
            State::StartOfRule(data) | State::EndOfRule(data) => Ok(data),
            _ => Err(Error::new("Unexpected end of rules block in bidi!")),
        }
    }
}

mod type_parser {
    use super::*;

    #[derive(Debug)]
    enum TypeNameState {
        Init,
        Ready(Vec<TokenTree>),
        C1(Vec<TokenTree>),
        C2(Vec<TokenTree>),
    }

    fn vec_with<T>(orig: &Vec<T>, new: &T) -> Vec<T>
    where T: Clone {
        let mut new_vec = orig.clone();
        new_vec.push(new.clone());
        new_vec
    }

    impl TypeNameState {
        pub fn next_step(&self, tt: &TokenTree) -> Result<Self> {
            use TypeNameState::*;
            use TokenTree::*;

            Ok(match (self, tt) {
                (Init, Ident(..)) =>
                    Ready(vec![tt.clone()]),

                (Init, Punct(ref p)) if p.as_char() == ':' && p.spacing() == Spacing::Joint =>
                    C1(vec![tt.clone()]),

                (C1(ref start), Punct(ref p)) if p.as_char() == ':' =>
                    C2(vec_with(start, tt)),

                (C2(ref start), Ident(..)) =>
                    Ready(vec_with(start, tt)),

                (Ready(ref start), Punct(ref p)) if p.as_char() == ':' && p.spacing() == Spacing::Joint =>
                    C1(vec_with(start, tt)),

                (_, to) => return Err(Error::new(&format!("Unexpected token {}", to)))
            })
        }

        pub fn extract(&self, to: &TokenTree) -> Result<TokenStream> {
            if let TypeNameState::Ready(vec) = self {
                Ok(TokenStream::from_iter(vec.clone()))
            } else {
                Err(Error::new(&format!("Unexpected token {}", to)))
            }
        }
    }

    #[derive(Debug)]
    enum State {
        Init(bool),
        Named(bool, TokenTree),
        ReadingInternal(bool, TokenTree, TypeNameState),
        ReadingExternal(bool, TokenTree, TokenStream, TypeNameState),
        Specified(bool, TokenTree, TokenStream, TokenStream),
        Done(model::Definition),
    }

    fn next_step(state: State, token: TokenTree) -> Result<State> {
        use State::*;
        use TokenTree::*;

        Ok(match (state, token.clone()) {
            (Init(for_tests), Ident(..)) => Named(for_tests, token),

            (Init(false), Punct(ref p)) if p.as_char() == '*' => Init(true),

            (Named(for_tests, ref name), Punct(ref p)) if p.as_char() == '<' =>
                ReadingInternal(for_tests, name.clone(), TypeNameState::Init),

            (ReadingInternal(for_tests, ref name, ref state), Punct(ref p)) if p.as_char() == ',' =>
                ReadingExternal(for_tests, name.clone(), state.extract(&token)?, TypeNameState::Init),

            (ReadingInternal(for_tests, name, state), token) =>
                ReadingInternal(for_tests, name, state.next_step(&token)?),

            (ReadingExternal(for_tests, ref name, ref internal, ref state), Punct(ref p)) if p.as_char() == '>' =>
                Specified(for_tests, name.clone(), internal.clone(), state.extract(&token)?),

            (ReadingExternal(for_tests, name, internal, state), token) =>
                ReadingExternal(for_tests, name, internal, state.next_step(&token)?),

            (Specified(for_tests, ref name, ref internal, ref external), Group(ref g)) if g.delimiter() == Delimiter::Brace => {
                Done(model::Definition {
                    for_tests: for_tests,
                    name: name.clone(),
                    internal_type: internal.clone(),
                    external_type: external.clone(),
                    rules: rules_parser::parse(g.stream())?,
                })
            }

            (_, to) => return Err(Error::new(&format!("Unexpected token {}", to))),
        })
    }

    pub(crate) fn parse(stream: TokenStream) -> Result<model::Definition> {
        let mut state = State::Init(false);

        for token in stream.into_iter() {
            state = next_step(state, token)?;
        }

        if let State::Done(definition) = state {
            Ok(definition)
        } else {
            Err(Error::new("Parse error at end of bidi!"))
        }
    }
}

fn mirror(direct: &model::Definition) -> model::Definition {
    use model::Rule::*;

    model::Definition {
        for_tests: direct.for_tests,
        name: direct.name.clone(),
        external_type: direct.internal_type.clone(),
        internal_type: direct.external_type.clone(),
        rules: direct
            .rules
            .iter()
            .map(|rule| match rule.clone() {
                Equivalence(internal, external) => Equivalence(external, internal),
                ProjectionToExternal(internal, external) => {
                    ProjectionToInternal(external, internal)
                }
                ProjectionToInternal(internal, external) => {
                    ProjectionToExternal(external, internal)
                }
                OrphanExternal(external) => OrphanInternal(external),
                OrphanInternal(internal) => OrphanExternal(internal),
            })
            .collect(),
    }
}

const INTERNAL_CONVERTIBLES_CONSTANT: &str = "INTERNAL_CONVERTIBLES";
const EXTERNAL_CONVERTIBLES_CONSTANT: &str = "EXTERNAL_CONVERTIBLES";

fn generate_convertibles_array(type_: &TokenStream, rules: &model::Rules) -> TokenStream {
    let names = rules
        .iter()
        .filter_map(|rule| match rule {
            model::Rule::Equivalence(internal, ..) => Some(internal),
            model::Rule::ProjectionToInternal(..) => None,
            model::Rule::ProjectionToExternal(internal, ..) => Some(internal),
            model::Rule::OrphanInternal(..) => None,
            model::Rule::OrphanExternal(..) => None,
        })
        .map(|variant| quote!(#type_::#variant));

    quote! {
        [
            #(#names ,)*
        ]
    }
}

fn generate_struct_impl(def: &model::Definition, mirror_rules: &model::Rules) -> TokenStream {
    let model::Definition {
        name,
        internal_type,
        external_type,
        rules: direct_rules,
        ..
    } = def;
    let int_constant_name = format_ident!("{}", INTERNAL_CONVERTIBLES_CONSTANT);
    let ext_constant_name = format_ident!("{}", EXTERNAL_CONVERTIBLES_CONSTANT);
    let int_constant = generate_convertibles_array(internal_type, direct_rules);
    let ext_constant = generate_convertibles_array(external_type, mirror_rules);

    quote! {
        impl #name {
            const #int_constant_name: &'static [#internal_type] = &#int_constant;
            const #ext_constant_name: &'static [#external_type] = &#ext_constant;
        }
    }
}

fn prefix(def: &model::Definition) -> TokenStream {
    if def.for_tests {
        quote!(crate)
    } else {
        quote!(::bidi)
    }
}

fn generate_partial_impl(def: &model::Definition, const_name: &str) -> TokenStream {
    let model::Definition {
        name,
        internal_type,
        external_type,
        rules,
        ..
    } = def;
    let constant_name = format_ident!("{}", const_name);

    let rules = rules.iter().filter_map(|rule| match rule {
        model::Rule::Equivalence(internal, external) => {
            Some(quote!(#internal_type::#internal => Some(#external_type::#external)))
        }
        model::Rule::ProjectionToInternal(..) => None,
        model::Rule::ProjectionToExternal(internal, external) => {
            Some(quote!(#internal_type::#internal => Some(#external_type::#external)))
        }
        model::Rule::OrphanInternal(internal) => Some(quote!(#internal_type::#internal => None)),
        model::Rule::OrphanExternal(..) => None,
    });

    let prefix = prefix(def);

    // TODO Refer to ::bidi:: instead of crate::
    quote! {
        impl #prefix::PartialEnumConverter<#internal_type, #external_type> for #name {
            fn convert_opt(internal_value: &#internal_type) -> Option<#external_type> {
                match internal_value {
                    #(#rules,)*
                }
            }

            fn convertible_values() -> &'static [#internal_type] {
                Self::#constant_name
            }
        }
    }
}

fn generate_complete_impl(def: &model::Definition) -> TokenStream {
    if def.rules.iter().all(|rule| match rule {
        model::Rule::Equivalence(..) => true,
        model::Rule::ProjectionToInternal(..) => true,
        model::Rule::ProjectionToExternal(..) => true,
        model::Rule::OrphanInternal(..) => false,
        model::Rule::OrphanExternal(..) => true,
    }) {
        let model::Definition {
            name,
            external_type,
            internal_type,
            ..
        } = def;
        let prefix = prefix(def);
        quote!(
            unsafe impl #prefix::EnumConverter<#internal_type, #external_type> for #name {}
        )
    } else {
        TokenStream::new()
    }
}

fn bidi_impl(item: proc_macro::TokenStream) -> Result<TokenStream> {
    let item = TokenStream::from(item);

    let definition = type_parser::parse(item)?;
    let mirror = mirror(&definition);

    let name = &definition.name;
    let out_definition = quote!( enum #name {} );

    let out_struct_impl = generate_struct_impl(&definition, &mirror.rules);
    let out_partial_impl1 = generate_partial_impl(&definition, INTERNAL_CONVERTIBLES_CONSTANT);
    let out_partial_impl2 = generate_partial_impl(&mirror, EXTERNAL_CONVERTIBLES_CONSTANT);
    let out_complete_impl1 = generate_complete_impl(&definition);
    let out_complete_impl2 = generate_complete_impl(&mirror);

    Ok(quote! {
        #out_definition
        #out_struct_impl
        #out_partial_impl1
        #out_complete_impl1
        #out_partial_impl2
        #out_complete_impl2
    })
}

#[proc_macro]
pub fn bidi(item: proc_macro::TokenStream) -> proc_macro::TokenStream {
    proc_macro::TokenStream::from(
        match bidi_impl(item) {
            Ok(stream) => stream,
            Err(error) => {
                let message = error.message;
                quote!(
                    ::std::compile_error!{#message}
                )
            }
        }
    )
}
