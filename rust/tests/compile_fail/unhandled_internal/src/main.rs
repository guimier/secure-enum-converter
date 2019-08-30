use bidi_codegen::*;
use bidi::*;

enum A { A1, A2, A3 }
enum B { B1, B2 }

bidi!(SUT<A, B> {
    A1 == B1,
    A2 == B2,
});

fn main() {
}
