#include "catch.hpp"

#include <maybe/result.hpp>

class A final {
public:
    A(std::string value) : value(value)
    {
    }
    std::string value;
};

class B final {
public:
    B(std::string value) : value(value)
    {
    }
    std::string value;
};

using maybe::result;

TEST_CASE("result_and_then")
{
    SECTION("chains another function returning different result if previous one was successful")
    {
        auto a = result<A, int>::ok(A("hello"));
        auto b = a.and_then([](A v) { return result<B, int>::ok(B(v.value + " world")); });
        REQUIRE(b);
        REQUIRE(b.ok_value().value == "hello world");
    }

    SECTION(
        "should not run another function returning different result if previous one returned error")
    {
        auto a = result<A, int>::err(43);
        auto b = a.and_then([](A v) { return result<B, int>::ok(B(v.value + " world")); });
        REQUIRE(!b);
        REQUIRE(b.err_value() == 43);
    }
}