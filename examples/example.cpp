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

void main() {
    auto success_result = run(true);

    assert(success_result);
    assert(success_result.ok_value() == 4);

    auto error_result = run(false);

    assert(!error_result);
    assert(error_result.err_value() == "file not found");
}