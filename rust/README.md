This Rust library is a work-in-progress procedural macro for generating bi-directional enum converter.

Example of what your code might look like:

```rust
use bidi_codegen::bidi;
use bidi::*;

enum A { A1, A2 };
enum B { B1, B2 };
bidi!(Converter<A, B> {
    A1 == B1,
    A2 == B2
});

fn main() {
    assert_eq!(
        Converter::convert(B::B1),
        A::A1
    );
}
```
