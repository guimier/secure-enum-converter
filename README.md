This C++ library provides a bi-directional enum converter with the two following
properties:

  - There is a single mapping between values written in the code. This way,
    you don’t have to care about updating two mappings at the same time.
  - The library won’t let you compile in case your mapping over- or
    underspecified relations.

Unfortunately this is not free and comes at the expense of:

  - Using unusual functionalities of C++. This library uses the preprocessor
    in ways that are often considered as bad design. Don’t use it if you don’t
    want to have weird macros and non-header `#include`s in your code.
  - Sometimes getting hard to understand compile error. This repository
    contains examples of good or almost-good compile errors, but with older
    compilers or some other errors, you might get really hard to understand
    errors.
  - Not using only standard C++. Currently only GCC is fully supported. While
    it is easy to make valid programs compile on other compilers, guaranteeing
    that the build fails for invalid programs might be harder.

The public API consists in the following three templates:

  - `SecureEnumConverter` is the base class, which implements the conversion.
  - `TaggedEnumConverter` provides a more natural way to call the converter
    methods, by using type tags instead of the “internal”/“external”
    terminology.
  - `TypedEnumConverter` provides an even simpler interface, which is
      probably the best for starting.

Example of what your code might look like:

```cpp
#include <cassert>
#include "lguim/secureenumconverter.h"

enum class A { A1, A2 };
enum class B { B1, B2 };
using Converter = lguim::TypedEnumConverter<A, B>;

#define SEC_TYPE Converter
#define SEC_MAPPING \
    SEC_EQUIV(A::A1, B::B1) \
    SEC_EQUIV(A::A2, B::B2)
#include "lguim/secureenumconverter.inc"

int main() {
    assert(
        A::A1
            ==
        Converter::convertOrThrow<A>(B::B1)
    );
}
```

The full documentation of how the `SEC_MAPPING` macro works can be found in the
class comment for `SecureEnumConverter` in `secureenumconverter.h`.
