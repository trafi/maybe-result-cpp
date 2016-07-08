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

#include "result.hpp"

template <typename T, typename E>
constexpr void maybe::result<T, E>::copy_from(const result<T, E>& other) noexcept
{
    using ::maybe::internal::Value;

    switch (other.tag) {
        case Value::OK:
            init_ok(other.ok_val);
            break;
        case Value::ERR:
            init_err(other.err_val);
            break;
        default:
            tag = Value::NONE;
            break;
    }
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::set_from(result<T, E>& other) noexcept
{
    using ::maybe::internal::Value;

    switch (other.tag) {
        case Value::OK:
            init_ok(std::move(other.ok_val));
            break;
        case Value::ERR:
            init_err(std::move(other.err_val));
            break;
        default:
            tag = Value::NONE;
            return;
    }
    other.tag = Value::NONE;
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::init_ok(T&& value) noexcept
{
    tag = ::maybe::internal::Value::OK;
    ::new (static_cast<void*>(ok_dataptr())) T(std::forward<T>(value));
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::init_ok(const T& value) noexcept
{
    tag = ::maybe::internal::Value::OK;
    ::new (static_cast<void*>(ok_dataptr())) T(value);
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::init_err(E&& value) noexcept
{
    tag = ::maybe::internal::Value::ERR;
    ::new (static_cast<void*>(err_dataptr())) E(std::forward<E>(value));
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::init_err(const E& value) noexcept
{
    tag = ::maybe::internal::Value::ERR;
    ::new (static_cast<void*>(err_dataptr())) E(value);
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::clear() noexcept
{
    using ::maybe::internal::Value;

    switch (tag) {
        case Value::OK:
            (&this->ok_val)->T::~T();
            tag = Value::NONE;
            break;
        case Value::ERR:
            (&this->err_val)->E::~E();
            tag = Value::NONE;
            break;
        default:
            break;
    }
}

template <typename T, typename E>
template <typename F>
constexpr auto maybe::result<T, E>::map(F f) noexcept
    -> maybe::result<typename std::result_of<F(T)>::type, E>
{
    typedef maybe::result<typename std::result_of<F(T)>::type, E> return_result_t;

    if (is_err()) {
        return return_result_t::err(std::forward<E>(err_value()));
    }
    return return_result_t::ok(f(std::forward<T>(ok_value())));
};

template <typename T, typename E>
template <typename U>
constexpr auto maybe::result<T, E>::map_value(U value) noexcept -> maybe::result<U, E>
{
    if (is_err()) {
        return maybe::result<U, E>::err(std::forward<E>(err_value()));
    }

    return maybe::result<U, E>::ok(std::forward<U>(value));
};

template <typename T, typename E>
template <typename F>
constexpr auto maybe::result<T, E>::map_err(F f) noexcept
    -> maybe::result<T, typename std::result_of<F(E)>::type>
{
    typedef maybe::result<T, typename std::result_of<F(E)>::type> return_result_t;

    if (is_ok()) {
        return return_result_t::ok(std::forward<T>(ok_value()));
    }
    return return_result_t::err(f(std::forward<E>(err_value())));
};

template <typename T, typename E>
template <typename U>
constexpr auto maybe::result<T, E>::map_err_value(U value) noexcept -> maybe::result<T, U>
{
    if (is_ok()) {
        return maybe::result<T, U>::ok(std::forward<T>(ok_value()));
    }

    return maybe::result<T, U>::err(std::forward<U>(value));
};

template <typename T, typename E>
template <typename F>
constexpr auto maybe::result<T, E>::and_then(F f) noexcept -> typename std::result_of<F(T)>::type
{
    typedef typename std::result_of<F(T)>::type result_t;

    if (is_err()) {
        return maybe::result<typename result_t::ok_type, E>::err(std::forward<E>(err_value()));
    }
    return f(std::forward<T>(ok_value()));
};

template <typename T, typename E>
template <typename R>
constexpr auto maybe::result<T, E>::into_err() noexcept -> R
{
    if (is_err()) {
        return R::err(std::forward<E>(err_value()));
    }
    return R::default_ok();
};