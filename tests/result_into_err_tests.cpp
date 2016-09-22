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
};

using maybe::result;

TEST_CASE("result_into_err")
{
    SECTION("converts result<A, E>::err to result<B, E>::err")
    {
        auto a = result<A, int>::err(42);
        auto b = a.into_err<result<B, int>>();
        REQUIRE(!b);
        REQUIRE(42 == b.err_value());
    }

    SECTION("converts result<A, E>::err to result<void, E>::err")
    {
        auto a = result<A, int>::err(42);
        auto b = a.into_err<void>();
        REQUIRE(!b);
        REQUIRE(42 == b.err_value());
    }
}