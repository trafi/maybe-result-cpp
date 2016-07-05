# Maybe Result

Maybe Result is a return value wrapper that can contain either a value
`T` or error `E`. It borrows ideas heavily from the [C++17's
`std::experimental::optional`][optional], [Rust's std::result](result) and
the [`std::expected`][expected] that was proposed but not yet accepted
for C++17.

[optional]: http://en.cppreference.com/w/cpp/experimental/optional
[result]: https://doc.rust-lang.org/std/result/
[expected]: https://github.com/ptal/expected

### How is it different from `std::experimental::optional`?

Has a value for error instead of `nullopt`.

### How is it different from `std::expected`?

It does not require the error to be an exception, and does not fall back to
exceptions. All values must be checked, similar to `std::experimental::optional`.

## Setting up

This is header-only library. We recommend to add `src` to included directories,
so that the include would be:

```cpp
#include <maybe/result.hpp>
```

A C++ compiler shat supports C++14 is required.
You can use `-std=c++14` flag for sufficiently recent versions of
`GCC` or `CLANG`.

__Warning! Library is highly experimental and is not guaranteed to work.__

## Running tests

```bash
cmake .
make tests && ./tests/tests
```

## License

Licensed under either of

 * Apache License, Version 2.0, ([LICENSE-APACHE](LICENSE-APACHE) or http://www.apache.org/licenses/LICENSE-2.0)
 * MIT license ([LICENSE-MIT](LICENSE-MIT) or http://opensource.org/licenses/MIT)

at your option.

### Contribution

Unless you explicitly state otherwise, any contribution intentionally
submitted for inclusion in the work by you, as defined in the Apache-2.0
license, shall be dual licensed as above, without any additional terms or
conditions.