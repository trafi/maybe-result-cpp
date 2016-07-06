#include "catch.hpp"

#include <maybe/result.hpp>

class A final {
public:
    A(std::string value) : value(value)
    {
    }
    std::string value;
};

using maybe::result;

TEST_CASE("result_map_err")
{
    SECTION("converts err bool to err int")
    {
        auto a = result<A, bool>::err(true);
        auto b = a.map_err([](bool v) { return v ? 42 : 43; });
        REQUIRE(!b);
        REQUIRE(b.err_value() == 42);
    }

    SECTION("does not convert anything and returns ok if not err")
    {
        auto a = result<A, bool>::ok(A("hi"));
        auto b = a.map_err([](bool v) { return v ? 42 : 43; });
        REQUIRE(b);
        REQUIRE(b.ok_value().value == "hi");
    }
}