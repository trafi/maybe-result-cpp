#include "catch.hpp"

#include <functional>
#include <maybe/result.hpp>
#include <memory>
#include <sstream>

using maybe::result;

class NoCopy final {
public:
    NoCopy(NoCopy&& other)
    {
        flag = other.flag;
        destructor_fun = std::move(other.destructor_fun);
        other.destructor_fun = nullptr;
    }
    NoCopy& operator=(NoCopy&& other) = default;

    NoCopy(int flag, std::function<void()> destructor_fun)
        : flag(flag), destructor_fun(destructor_fun)
    {
    }
    ~NoCopy()
    {
        if (destructor_fun != nullptr) {
            destructor_fun();
        }
    }

    int get_flag()
    {
        return this->flag;
    }

private:
    int flag;
    std::function<void()> destructor_fun;
};

TEST_CASE("maybe_result_tests")
{
    SECTION("created ok result returns ok value")
    {
        auto res = result<int, int>::ok(12);
        REQUIRE(res.is_ok());
        REQUIRE(!res.is_err());
        REQUIRE(res);
        REQUIRE(res.ok_value() == 12);

        auto other_ok = result<int, int>::ok(12);
        REQUIRE(res == other_ok);

        auto other_different_ok = result<int, int>::ok(42);
        REQUIRE(res != other_different_ok);
    }

    SECTION("created err result returns err value")
    {
        auto res = result<int, int>::err(12);
        REQUIRE(res.is_err());
        REQUIRE(!res.is_ok());
        REQUIRE(!res);
        REQUIRE(res.err_value() == 12);

        auto other_err = result<int, int>::err(12);
        REQUIRE(res == other_err);

        auto other_different_err = result<int, int>::err(42);
        REQUIRE(res != other_different_err);
    }

    SECTION("created ok not equal created result")
    {
        auto ok = result<int, int>::ok(12);
        auto err = result<int, int>::err(12);
        REQUIRE(ok != err);
    }

        SECTION("throws exception if invalid value accessed")
        {
            auto ok = result<int, int>::ok(12);
            auto err = result<int, int>::err(12);
            REQUIRE_THROWS(ok.err_value());
            REQUIRE_THROWS(err.ok_value());
        }

    SECTION("returns default values")
    {
        auto ok = result<int, int>::ok(12);
        auto err = result<int, int>::err(12);
        REQUIRE(ok.ok_value_or(42) == 12);
        REQUIRE(err.err_value_or(42) == 12);
        REQUIRE(ok.err_value_or(42) == 42);
        REQUIRE(err.ok_value_or(42) == 42);
    }

    SECTION("destroys ok value correctly")
    {
        std::ostringstream ss;

        {
            result<NoCopy, NoCopy>::ok(NoCopy(0, [&] { ss << "[ok]"; }));
        }

        REQUIRE(ss.str() == "[ok]");
    }

    SECTION("destroys err value correctly")
    {
        std::ostringstream ss;

        {
            result<NoCopy, NoCopy>::err(NoCopy(0, [&] { ss << "[err]"; }));
        }

        REQUIRE(ss.str() == "[err]");
    }

    SECTION("nocopy ok to ok is moved correctly")
    {
        std::ostringstream ss;

        {
            auto val = result<NoCopy, NoCopy>::ok(NoCopy(0, [&] { ss << "[first]"; }));
            val = result<NoCopy, NoCopy>::ok(NoCopy(42, [&] { ss << "[second]"; }));
            REQUIRE(val.ok_value().get_flag() == 42);
        }

        REQUIRE(ss.str() == "[first][second]");
    }

    SECTION("nocopy err to err is moved correctly")
    {
        std::ostringstream ss;

        {
            auto val = result<NoCopy, NoCopy>::err(NoCopy(0, [&] { ss << "[first]"; }));
            val = result<NoCopy, NoCopy>::err(NoCopy(42, [&] { ss << "[second]"; }));
            REQUIRE(val.err_value().get_flag() == 42);
        }

        REQUIRE(ss.str() == "[first][second]");
    }

    SECTION("nocopy ok to err is moved correctly")
    {
        std::ostringstream ss;

        {
            auto val = result<NoCopy, NoCopy>::err(NoCopy(0, [&] { ss << "[first]"; }));
            val = result<NoCopy, NoCopy>::ok(NoCopy(42, [&] { ss << "[second]"; }));
            REQUIRE(val.ok_value().get_flag() == 42);
        }

        REQUIRE(ss.str() == "[first][second]");
    }

    SECTION("nocopy err to ok is moved correctly")
    {
        std::ostringstream ss;

        {
            auto val = result<NoCopy, NoCopy>::ok(NoCopy(0, [&] { ss << "[first]"; }));
            val = result<NoCopy, NoCopy>::err(NoCopy(42, [&] { ss << "[second]"; }));
            REQUIRE(val.err_value().get_flag() == 42);
        }

        REQUIRE(ss.str() == "[first][second]");
    }

    SECTION("nocopy ok to none is moved correctly")
    {
        std::ostringstream ss;

        {
            result<NoCopy, NoCopy> val;
            val = result<NoCopy, NoCopy>::ok(NoCopy(42, [&] { ss << "[ok]"; }));
            REQUIRE(val.ok_value().get_flag() == 42);
        }

        REQUIRE(ss.str() == "[ok]");
    }

    SECTION("nocopy err to none is moved correctly")
    {
        std::ostringstream ss;

        {
            result<NoCopy, NoCopy> val;
            val = result<NoCopy, NoCopy>::err(NoCopy(42, [&] { ss << "[err]"; }));
            REQUIRE(val.err_value().get_flag() == 42);
        }

        REQUIRE(ss.str() == "[err]");
    }
}