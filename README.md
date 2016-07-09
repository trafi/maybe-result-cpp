# Maybe Result

[![Build Status](https://travis-ci.org/trafi/maybe-result-cpp.svg?branch=master)](https://travis-ci.org/trafi/maybe-result-cpp)

Maybe Result is a return value wrapper that can contain either a value
`T` or error `E`. It borrows ideas heavily from the [C++17's
`std::experimental::optional`][optional], [Rust's std::result][result] and
the [`std::expected`][expected] that was proposed but not yet accepted
for C++17.

[optional]: http://en.cppreference.com/w/cpp/experimental/optional
[result]: https://doc.rust-lang.org/std/result/
[expected]: https://github.com/ptal/expected

## Example

```cpp
#include <maybe/result.hpp>
#include <vector>

using maybe::result;
using namespace std;

/**
 * Error type.
 */
enum class LoadError {
    FileNotFound,
};

/**
 * Example function that returns a successful result or error depending on param.
 */
result<vector<string>, LoadError> load_names(bool return_successfully)
{
    if (return_successfully) {
        return result<vector<string>, LoadError>::ok({"Bob", "Alice"});
    } else {
        return result<vector<string>, LoadError>::err(LoadError::FileNotFound);
    }
}

/**
 * Chain both functions and merge results into one.
 *
 * If `return_success_from_first` is false, the fist function will fail.
 */
result<size_t, string> run(bool return_success_from_first)
{
    return load_names(success) // function will fail depending on flag
        // run this if previous succeeds
        .and_then([](auto prev_names) {
            // combine results of both
            return load_names(true).map([prev_names](auto more_names) {
                vector<string> all;
                copy(prev_names.begin(), prev_names.end(), back_inserter(all));
                copy(more_names.begin(), more_names.end(), back_inserter(all));
                return all;
            });
        })
        // change error from LoadError to string
        .map_err([](auto err) {
            if (err == LoadError::FileNotFound) {
                return string("file not found");
            }
            return string("other error");
        })
        // map result value to the number of elements in vector
        .map([](vector<string> results) { return results.size(); });
}

auto success_result = run(true);

REQUIRE(success_result);
REQUIRE(success_result.ok_value() == 4);

auto error_result = run(false);

REQUIRE(!error_result);
REQUIRE(error_result.err_value() == "file not found");

```

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
`GCC` (4.9) or `CLANG` (3.7).

__Warning! Library is highly experimental and is not guaranteed to work.__

## Running tests

Library requires `std::experimental::optional` implementation, location
of which can be specified with `-DEXPERIMENTAL_OPTIONAL_INCLUDE` flag:

```bash
cmake -DEXPERIMENTAL_OPTIONAL_INCLUDE=../path/to/optional .
make tests && ./tests/tests
```

There is a script that does this automatically:

```bash
./dev/run-tests.sh
```

In addition to this, you can run tests on all supported compilers using docker:

```bash
./dev/docker-run-tests.sh
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