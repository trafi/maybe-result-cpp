#include "catch.hpp"

#include <maybe/result.hpp>

class A final {
public:
    A(std::string value) : value(value) {}
    std::string value;
};

class B final {
public:
    B(std::string value) : value(value) {}
    std::string value;
};

using maybe::result;

TEST_CASE("result_chaining")
{
    SECTION("converts result A to result B")
    {
        auto a = result<A, int>::ok(A("hello"));
        auto b = a.map([](A v) { return B(v.value); });
        REQUIRE(b);
        REQUIRE(b.ok_value().value == "hello");
    }
}