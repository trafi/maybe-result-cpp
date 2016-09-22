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

TEST_CASE("result_map")
{
    SECTION("converts result A to result B")
    {
        auto a = result<A, int>::ok(A("hello"));
        auto b = a.map([](A v) { return B(v.value + " world"); });
        REQUIRE(b);
        REQUIRE(b.ok_value().value == "hello world");
    }

    SECTION("converts result void to result B")
    {
        auto a = result<void, int>::ok();
        auto b = a.map([]() { return B("Void world"); });
        REQUIRE(b);
        REQUIRE(b.ok_value().value == "Void world");
    }

    SECTION("does not convert result A to result B if value was error")
    {
        auto a = result<A, int>::err(43);
        auto b = a.map([](A v) { return B(v.value = " world"); });
        REQUIRE(!b);
        REQUIRE(43 == b.err_value());
    }

    SECTION("does not convert result A to result void if value was error")
    {
        auto a = result<A, int>::err(43);
        auto b = a.map_void();
        REQUIRE(!b);
        REQUIRE(43 == b.err_value());
    }

    SECTION("replaces result A with result B")
    {
        auto a = result<A, int>::ok(A("hello"));
        auto b = a.map_value(B("bye"));
        REQUIRE(b);
        REQUIRE(b.ok_value().value == "bye");
    }

    SECTION("does not replace result A with result B if A is err")
    {
        auto a = result<A, int>::err(43);
        auto b = a.map_value(B("bye"));
        REQUIRE(!b);
        REQUIRE(43 == b.err_value());
    }
}