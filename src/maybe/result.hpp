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

#include "result.fwd.hpp"

#include <string>
#include <optional.hpp>

namespace maybe {
    namespace internal {
        struct placeholder {
        };
    }

    template <typename T, typename E>
    class result final {
    private:
        std::experimental::optional<T> var_ok;
        std::experimental::optional<E> var_err;

    public:
        typedef T ok_type;
        typedef E err_type;

        result()
        {
        }

        result(T&& value, internal::placeholder) : var_ok(std::forward<T>(value))
        {
        }

        result(internal::placeholder, E&& value) : var_err(std::forward<E>(value))
        {
        }

        /** helper constructor for ok value */
        constexpr static result<T, E> ok(T&& value) noexcept
        {
            return std::forward<result<T, E>>(
                result<T, E>(std::forward<T>(value), internal::placeholder{}));
        }

        /** helper constructor for ok value */
        constexpr static result<T, E> err(E&& value) noexcept
        {
            return std::forward<result<T, E>>(
                result<T, E>(internal::placeholder{}, std::forward<E>(value)));
        }

        // Inspection.

        inline bool is_ok() const noexcept
        {
            return !!var_ok;
        }

        inline bool is_err() const noexcept
        {
            return !!var_err;
        }

        explicit inline operator bool() const noexcept
        {
            return is_ok();
        }

#if OPTIONAL_HAS_MOVE_ACCESSORS == 1

        constexpr T const& ok_value() const&
        {
            return var_ok.value();
        }

        OPTIONAL_MUTABLE_CONSTEXPR T& ok_value() &
        {
            return var_ok.value();
        }

        OPTIONAL_MUTABLE_CONSTEXPR T&& ok_value() &&
        {
            return std::move(var_ok.value());
        }

#else

        T const& ok_value() const
        {
            return var_ok.value();
        }

        T& ok_value()
        {
            return var_ok.value();
        }

#endif

#if OPTIONAL_HAS_THIS_RVALUE_REFS == 1

        template <class V>
        constexpr T ok_value_or(V&& v) const&
        {
            return var_ok.value_or(std::forward<V>(v));
        }

        template <class V>
        OPTIONAL_MUTABLE_CONSTEXPR T ok_value_or(V&& v) &&
        {
            return var_err.value_or(std::forward<V>(v));
        }

#endif

#if OPTIONAL_HAS_MOVE_ACCESSORS == 1

        constexpr E const& err_value() const&
        {
            return var_err.value();
        }

        OPTIONAL_MUTABLE_CONSTEXPR E& err_value() &
        {
            return var_err.value();
        }

        OPTIONAL_MUTABLE_CONSTEXPR E&& err_value() &&
        {
            return std::move(var_err.value());
        }

#else

        E const& err_value() const
        {
            return var_err.value();
        }

        E& err_value()
        {
            return var_err.value();
        }

#endif

#if OPTIONAL_HAS_THIS_RVALUE_REFS == 1

        template <class V>
        constexpr E err_value_or(V&& v) const&
        {
            return var_err.value_or(std::forward<V>(v));
        }

        template <class V>
        OPTIONAL_MUTABLE_CONSTEXPR E err_value_or(V&& v) &&
        {
            return var_err.value_or(std::forward<V>(v));
        }

#endif

        // Functional helpers.

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
        inline auto map(F f) noexcept -> maybe::result<typename std::result_of<F(T)>::type, E>;

        /**
         * Maps a result<T, E> to result<U, E> by always returning provided U value on success,
         * leaving an err value untouched.
         *
         * This function can be used to compose the results of two functions.
         *
         * @param value U
         * @return maybe::result<U, E>
         */
        template <typename U>
        inline auto map_value(U value) noexcept -> maybe::result<U, E>;

        /**
         * Maps a result<T, E> to result<T, U> (where U is return value of F(E)) by applying a
         * function to a
         * function to a
         * contained err value, leaving an ok value untouched.
         *
         * This function can be used to pass through a successful result while changing an error.
         *
         * @param f F(E) -> U
         * @return maybe::result<T, U>
         */
        template <typename F>
        inline auto map_err(F f) noexcept -> maybe::result<T, typename std::result_of<F(E)>::type>;

        /**
         * Maps a result<T, E> to result<T, U> by always returning provided U value on error,
         * leaving an ok value untouched.
         *
         * This function can be used to compose the results of two functions.
         *
         * @param value U
         * @return maybe::result<T, U>
         */
        template <typename U>
        inline auto map_err_value(U value) noexcept -> maybe::result<T, U>;

        /**
         * Calls op if the result is ok, otherwise returns the err value of self.
         *
         * This function can be used for control flow based on result values.
         *
         * @param f F(T) -> maybe::result<U, E>
         * @return maybe::result<U, E>
         */
        template <typename F>
        inline auto and_then(F op) noexcept -> typename std::result_of<F(T)>::type;

        template <typename R>
        inline auto into_err() noexcept -> R;
    };

    template <typename T, typename E>
    constexpr bool operator==(const result<T, E>& x, const result<T, E>& y)
    {
        return x.is_ok() && y.is_ok()
            ? x.ok_value() == y.ok_value()
            : (x.is_err() && y.is_err() ? x.err_value() == y.err_value() : false);
    }

    template <typename T, typename E>
    constexpr bool operator!=(const result<T, E>& x, const result<T, E>& y)
    {
        return !(x == y);
    }
}

#include "result.inline.hpp"