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

#include <cassert>
#include <stdexcept>
#include <string>

#include "platforms.hpp"
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
    inline constexpr T&& mr_constexpr_forward(typename std::remove_reference<T>::type& t) noexcept
    {
        return static_cast<T&&>(t);
    }

    template <class T>
    inline constexpr T&& mr_constexpr_forward(typename std::remove_reference<T>::type&& t) noexcept
    {
        static_assert(!std::is_lvalue_reference<T>::value, "!!");
        return static_cast<T&&>(t);
    }

    template <class T>
    inline constexpr typename std::remove_reference<T>::type&& mr_constexpr_move(T&& t) noexcept
    {
        return static_cast<typename std::remove_reference<T>::type&&>(t);
    }

    template <typename T, typename E>
    class result final {
    private:
        void copy_from(const result<T, E>& other) noexcept;
        void set_from(result<T, E>& other) noexcept;
        void init_ok(T&& value) noexcept;
        void init_ok(const T& value) noexcept;
        void init_err(E&& value) noexcept;
        void init_err(const E& value) noexcept;
        void clear() noexcept;

        internal::Value tag;
        union {
            T ok_val;
            E err_val;
        };

    public:
        typedef T ok_type;
        typedef T err_type;

        result() : tag(internal::Value::NONE)
        {
        }

        // Internal initialization.

        /** Ok initializer */
        result(T&& value, internal::Placeholder) : tag(internal::Value::OK)
        {
            init_ok(std::forward<T>(value));
        }

        /** Ok initializer */
        result(const T& value, internal::Placeholder) : tag(internal::Value::OK)
        {
            init_ok(value);
        }

        /** Err initializer */
        result(internal::Placeholder, E&& value) : tag(internal::Value::ERR)
        {
            init_err(std::forward<E>(value));
        }

        /** Err initializer */
        result(internal::Placeholder, const E& value) : tag(internal::Value::ERR)
        {
            init_err(value);
        }

        // Set up and tear down.

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

        // Static construction helpers.

        /** helper constructor for default ok value */
        constexpr static result<T, E> default_ok() noexcept
        {
            return mr_constexpr_move(result<T, E>(T(), internal::Placeholder{}));
        }

        /** helper constructor for ok value */
        constexpr static result<T, E> ok(T&& value) noexcept
        {
            return mr_constexpr_move(result<T, E>(std::forward<T>(value), internal::Placeholder{}));
        }

        /** helper constructor for ok value */
        constexpr static result<T, E> ok(const T& value) noexcept
        {
            return mr_constexpr_move(result<T, E>(value, internal::Placeholder{}));
        }

        /** helper constructor for default err value */
        constexpr static result<T, E> default_err() noexcept
        {
            return mr_constexpr_move(result<T, E>(internal::Placeholder{}, E()));
        }

        /** helper constructor for err value */
        constexpr static result<T, E> err(E&& err) noexcept
        {
            return mr_constexpr_move(result<T, E>(internal::Placeholder{}, std::forward<E>(err)));
        }

        /** helper constructor for err value */
        constexpr static result<T, E> err(const E& err) noexcept
        {
            return mr_constexpr_move(result<T, E>(internal::Placeholder{}, err));
        }

#if MAYBE_RESULT_HAS_MOVE_ACCESSORS == 1

        MAYBE_RESULT_MUTABLE_CONSTEXPR T* operator->()
        {
            return ok_dataptr();
        }

        MAYBE_RESULT_MUTABLE_CONSTEXPR T& operator*() &
        {
            return *ok_dataptr();
        }

        MAYBE_RESULT_MUTABLE_CONSTEXPR T&& operator*() &&
        {
            return mr_constexpr_move(*ok_dataptr());
        }

        constexpr T const& ok_value() const&
        {
            return is_ok() ? ok_val : (throw bad_result_access("bad ok result access"), ok_val);
        }

        MAYBE_RESULT_MUTABLE_CONSTEXPR T& ok_value() &
        {
            return is_ok() ? ok_val : (throw bad_result_access("bad ok result access"), ok_val);
        }

        MAYBE_RESULT_MUTABLE_CONSTEXPR T&& ok_value() &&
        {
            if (!is_ok())
                throw bad_result_access("bad ok result access");
            return std::move(ok_val);
        }

        constexpr E const& err_value() const&
        {
            return is_err() ? err_val : (throw bad_result_access("bad err result access"), err_val);
        }

        MAYBE_RESULT_MUTABLE_CONSTEXPR E& err_value() &
        {
            return is_err() ? err_val : (throw bad_result_access("bad err result access"), err_val);
        }

        MAYBE_RESULT_MUTABLE_CONSTEXPR E&& err_value() &&
        {
            if (!is_err())
                throw bad_result_access("bad err result access");
            return std::move(err_val);
        }

#else

        T* operator->()
        {
            return ok_dataptr();
        }

        T& operator*()
        {
            return *ok_dataptr();
        }

        constexpr T const& ok_value() const
        {
            return is_ok() ? ok_val : (throw bad_result_access("bad ok result access"), ok_val);
        }

        T& ok_value()
        {
            return is_ok() ? ok_val : (throw bad_result_access("bad ok result access"), ok_val);
        }

        constexpr E const& err_value() const
        {
            return is_err() ? err_val : (throw bad_result_access("bad err result access"), err_val);
        }

        E& err_value()
        {
            return is_err() ? err_val
                            : (throw bad_result_access("bad err result access"), err_val);
        }

#endif

#if MAYBE_RESULT_HAS_THIS_RVALUE_REFS == 1

        template <class V>
        constexpr T ok_value_or(V&& v) const&
        {
            return is_ok() ? ok_val : static_cast<T>(mr_constexpr_forward<V>(v));
        }

        template <class V>
        constexpr E err_value_or(V&& v) const&
        {
            return is_err() ? err_val : static_cast<E>(mr_constexpr_forward<V>(v));
        }

#if MAYBE_RESULT_HAS_MOVE_ACCESSORS == 1

        template <class V>
        MAYBE_RESULT_MUTABLE_CONSTEXPR T ok_value_or(V&& v) &&
        {
            return is_ok() ? mr_constexpr_move(const_cast<result<T, E>&>(*this).ok_val)
                           : static_cast<T>(mr_constexpr_forward<V>(v));
        }

        template <class V>
        MAYBE_RESULT_MUTABLE_CONSTEXPR E err_value_or(V&& v) &&
        {
            return is_err() ? mr_constexpr_move(const_cast<result<T, E>&>(*this).err_val)
                            : static_cast<E>(mr_constexpr_forward<V>(v));
        }

#else

        template <class V>
        T ok_value_or(V&& v) &&
        {
            return is_ok() ? mr_constexpr_move(const_cast<result<T, E>&>(*this).ok_val)
                           : static_cast<T>(mr_constexpr_forward<V>(v));
        }

        template <class V>
        E err_value_or(V&& v) &&
        {
            return is_err() ? mr_constexpr_move(const_cast<result<T, E>&>(*this).err_val)
                            : static_cast<E>(mr_constexpr_forward<V>(v));
        }

#endif

#else

        template <class V>
        constexpr T ok_value_or(V&& v) const
        {
            return is_ok() ? ok_val : static_cast<T>(mr_constexpr_forward<V>(v));
        }

        template <class V>
        constexpr E err_value_or(V&& v) const
        {
            return is_err() ? err_val : static_cast<T>(mr_constexpr_forward<V>(v));
        }

#endif

        // Inspection.

        constexpr bool is_ok() const noexcept
        {
            return tag == internal::Value::OK;
        }

        constexpr bool is_err() const noexcept
        {
            return tag == internal::Value::ERR;
        }

        explicit constexpr operator bool() const noexcept
        {
            return is_ok();
        }

        // Unsafe access.

        constexpr T* ok_dataptr() noexcept
        {
            assert(is_ok());
            return const_cast<T*>(std::addressof(ok_val));
        }

        constexpr E* err_dataptr() noexcept
        {
            assert(is_err());
            return const_cast<E*>(std::addressof(err_val));
        }

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
        auto map(F f) noexcept -> maybe::result<typename std::result_of<F(T)>::type, E>;

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
        auto map_value(U value) noexcept -> maybe::result<U, E>;

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
        auto map_err(F f) noexcept
            -> maybe::result<T, typename std::result_of<F(E)>::type>;

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
        auto map_err_value(U value) noexcept -> maybe::result<T, U>;

        /**
         * Calls op if the result is ok, otherwise returns the err value of self.
         *
         * This function can be used for control flow based on result values.
         *
         * @param f F(T) -> maybe::result<U, E>
         * @return maybe::result<U, E>
         */
        template <typename F>
        auto and_then(F op) noexcept -> typename std::result_of<F(T)>::type;

        template <typename R>
        auto into_err() noexcept -> R;
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