/*
 * Copyright 2016 TRAFI
 *
 * Licensed under the Apache License, Version 2.0, <LICENSE-APACHE or
 * http://apache.org/licenses/LICENSE-2.0> or the MIT license <LICENSE-MIT or
 * http://opensource.org/licenses/MIT>, at your option. This file may not be
 * copied, modified, or distributed except according to those terms.
 *
 */

#pragma once

#include <stdexcept>
#include <string>

#include "result.fwd.hpp"

namespace maybe {

    namespace internal {
        enum class Value { OK, ERR, NONE };
        struct Placeholder {
        };
    }

    class bad_result_access : public std::logic_error {
    public:
        explicit bad_result_access(const std::string& what_arg) : logic_error{what_arg}
        {
        }
        explicit bad_result_access(const char* what_arg) : logic_error{what_arg}
        {
        }
    };

    // workaround: std utility functions aren't constexpr yet
    template <class T>
    inline constexpr T&& constexpr_forward(typename std::remove_reference<T>::type& t) noexcept
    {
        return static_cast<T&&>(t);
    }

    template <class T>
    inline constexpr T&& constexpr_forward(typename std::remove_reference<T>::type&& t) noexcept
    {
        static_assert(!std::is_lvalue_reference<T>::value, "!!");
        return static_cast<T&&>(t);
    }

    template <typename T, typename E>
    class result final {
    public:
        typedef T ok_type;

        result() : tag(internal::Value::NONE)
        {
        }

        /** ok initializer */
        result(T&& value, internal::Placeholder) : tag(internal::Value::OK)
        {
            init_ok(std::forward<T>(value));
        }

        /** ok initializer */
        result(const T& value, internal::Placeholder) : tag(internal::Value::OK)
        {
            init_ok(value);
        }

        /** err initializer */
        result(internal::Placeholder, E&& value) : tag(internal::Value::ERR)
        {
            init_err(std::forward<E>(value));
        }

        /** err initializer */
        result(internal::Placeholder, const E& value) : tag(internal::Value::ERR)
        {
            init_err(value);
        }

        /** copy constructor */
        result(const result& other) noexcept
        {
            copy_from(other);
        }

        /** copy assignment */
        result& operator=(const result& other) noexcept
        {
            clear();
            copy_from(other);
            return *this;
        }

        /** move constructor */
        result(result&& other) noexcept
        {
            set_from(other);
        }

        /** move assignment */
        result& operator=(result&& other) noexcept
        {
            clear();
            set_from(other);
            return *this;
        }

        /** destructor */
        ~result<T, E>()
        {
            clear();
        };

        /** helper constructor for default ok value */
        constexpr static result<T, E> default_ok() noexcept
        {
            return result<T, E>(T(), internal::Placeholder{});
        }

        /** helper constructor for ok value */
        constexpr static result<T, E> ok(T&& value) noexcept
        {
            return result<T, E>(std::forward<T>(value), internal::Placeholder{});
        }

        /** helper constructor for ok value */
        constexpr static result<T, E> ok(const T& value) noexcept
        {
            return result<T, E>(value, internal::Placeholder{});
        }

        /** helper constructor for default err value */
        constexpr static result<T, E> default_err() noexcept
        {
            return result<T, E>(internal::Placeholder{}, E());
        }

        /** helper constructor for err value */
        constexpr static result<T, E> err(E&& err) noexcept
        {
            return result<T, E>(internal::Placeholder{}, std::forward<E>(err));
        }

        /** helper constructor for err value */
        constexpr static result<T, E> err(const E& err) noexcept
        {
            return result<T, E>(internal::Placeholder{}, err);
        }

        constexpr bool is_ok() const noexcept
        {
            return tag == internal::Value::OK;
        }

        constexpr bool is_err() const noexcept
        {
            return tag == internal::Value::ERR;
        }

        constexpr T const& ok_value() const&
        {
            return is_ok() ? ok_val : (throw bad_result_access("bad ok result access"), ok_val);
        }

        constexpr T& ok_value() &
        {
            return is_ok() ? ok_val : (throw bad_result_access("bad ok result access"), ok_val);
        }

        constexpr E const& err_value() const&
        {
            return is_err() ? err_val : (throw bad_result_access("bad err result access"), err_val);
        }

        constexpr E& err_value() &
        {
            return is_err() ? err_val : (throw bad_result_access("bad err result access"), err_val);
        }

        template <class V>
        constexpr T ok_value_or(V&& v) const&
        {
            return is_ok() ? ok_val : static_cast<T>(constexpr_forward<V>(v));
        }

        template <class V>
        constexpr E err_value_or(V&& v) const&
        {
            return is_err() ? err_val : static_cast<E>(constexpr_forward<V>(v));
        }

        explicit constexpr operator bool() const noexcept
        {
            return is_ok();
        }

        constexpr T* ok_dataptr() const noexcept
        {
            return std::addressof(ok_val);
        }

        constexpr E* err_dataptr() const noexcept
        {
            return std::addressof(err_val);
        }

        /**
         * Maps a result<T, E> to result<U, E> (where U is return value of F(T)) by applying a
         * function F to a
         * contained ok value, leaving an err value untouched.
         *
         * This function can be used to compose the results of two functions.
         *
         * @param f F(T) -> U
         * @return maybe::result<U, E>
         */
        template <typename F>
        constexpr auto map(F f) noexcept -> maybe::result<typename std::result_of<F(T)>::type, E>;

        template <typename U>
        constexpr auto map_value(U value) noexcept -> maybe::result<U, E>;

        /**
         * Maps a result<T, E> to result<T, U> (where U is return value of F(E)) by applying a
         * function to a
         * contained err value, leaving an ok value untouched.
         *
         * This function can be used to pass through a successful result while changing an error.
         *
         * @param f F(E) -> U
         * @return maybe::result<T, U>
         */
        template <typename F>
        constexpr auto map_err(F f) noexcept
            -> maybe::result<T, typename std::result_of<F(E)>::type>;

        /**
         * Calls op if the result is ok, otherwise returns the err value of self.
         *
         * This function can be used for control flow based on result values.
         *
         * @param f F(T) -> maybe::result<U, E>
         * @return maybe::result<U, E>
         */
        template <typename F>
        constexpr auto and_then(F op) noexcept -> typename std::result_of<F(T)>::type;

        template <typename R>
        constexpr auto into_err() noexcept -> R
        {
            if (is_err()) {
                return R::err(std::forward<E>(err_value()));
            }
            return R::default_ok();
        };

    private:
        constexpr void copy_from(const result<T, E>& other) noexcept;
        constexpr void set_from(result<T, E>& other) noexcept;
        constexpr void init_ok(T&& value) noexcept;
        constexpr void init_ok(const T& value) noexcept;
        constexpr void init_err(E&& value) noexcept;
        constexpr void init_err(const E& value) noexcept;
        constexpr void clear() noexcept;

        internal::Value tag;
        union {
            T ok_val;
            E err_val;
        };
    };

    template <typename T, typename E>
    constexpr bool operator==(const result<T, E>& x, const result<T, E>& y)
    {
        return x.is_ok() && y.is_ok()
            ? *x.ok_dataptr() == *y.ok_dataptr()
            : (x.is_err() && y.is_err() ? *x.err_dataptr() == *y.err_dataptr() : false);
    }

    template <typename T, typename E>
    constexpr bool operator!=(const result<T, E>& x, const result<T, E>& y)
    {
        return !(x == y);
    }
}

#include "result.inline.hpp"