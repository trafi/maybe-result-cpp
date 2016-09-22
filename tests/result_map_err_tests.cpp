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

TEST_CASE("result_map_err")
{
    SECTION("converts err bool to err int")
    {
        auto a = result<A, bool>::err(true);
        auto b = a.map_err([](bool v) { return v ? 42 : 43; });
        REQUIRE(!b);
        REQUIRE(42 == b.err_value());
    }

    SECTION("converts err bool to err int for void function")
    {
        auto a = result<void, bool>::err(true);
        auto b = a.map_err([](bool v) { return v ? 42 : 43; });
        REQUIRE(!b);
        REQUIRE(42 == b.err_value());
    }

    SECTION("does not convert anything and returns ok if not err")
    {
        auto a = result<A, bool>::ok(A("hi"));
        auto b = a.map_err([](bool v) { return v ? 42 : 43; });
        REQUIRE(b);
        REQUIRE(b.ok_value().value == "hi");
    }

    SECTION("does not convert anything and returns ok if not err for void ok type")
    {
        auto a = result<void, bool>::ok();
        auto b = a.map_err([](bool v) { return v ? 42 : 43; });
        REQUIRE(b);
    }

    SECTION("replaces err A with err B")
    {
        auto a = result<int, A>::err(A("hello"));
        auto b = a.map_err_value(B("bye"));
        REQUIRE(!b);
        REQUIRE(b.err_value().value == "bye");
    }

    SECTION("replaces err A with err B for void ok type")
    {
        auto a = result<void, A>::err(A("hello"));
        auto b = a.map_err_value(B("bye"));
        REQUIRE(!b);
        REQUIRE(b.err_value().value == "bye");
    }

    SECTION("does not replace err A with err B if A is ok")
    {
        auto a = result<int, A>::ok(43);
        auto b = a.map_err_value(B("bye"));
        REQUIRE(b);
        REQUIRE(43 == b.ok_value());
    }

    SECTION("does not replace err A with err B if A is void ok")
    {
        auto a = result<void, A>::ok();
        auto b = a.map_err_value(B("bye"));
        REQUIRE(b);
    }
}