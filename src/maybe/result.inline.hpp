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
template <typename F, typename R>
inline auto maybe::result<T, E>::map(F f) noexcept -> maybe::result<R, E>
{
    if (is_err()) {
        return maybe::result<R, E>::err(std::forward<E>(err_value()));
    }

    return maybe::result<R, E>(f(ok_value()), internal::placeholder{});
};

template <typename T, typename E>
inline auto maybe::result<T, E>::map_void() noexcept -> maybe::result<void, E>
{
    if (is_err()) {
        return maybe::result<void, E>::err(std::forward<E>(err_value()));
    }

    return maybe::result<void, E>();
};

template <typename T, typename E>
template <typename U>
inline auto maybe::result<T, E>::map_value(U value) noexcept -> maybe::result<U, E>
{
    if (is_err()) {
        return maybe::result<U, E>::err(std::forward<E>(err_value()));
    }

    return maybe::result<U, E>(std::forward<U>(value), internal::placeholder{});
};

template <typename T, typename E>
template <typename F>
inline auto maybe::result<T, E>::map_err(F f) noexcept
    -> maybe::result<T, typename std::result_of<F(E)>::type>
{
    typedef maybe::result<T, typename std::result_of<F(E)>::type> return_result_t;

    if (is_ok()) {
        return return_result_t(std::forward<T>(ok_value()), internal::placeholder{});
    }
    return return_result_t(internal::placeholder{}, f(err_value()));
};

template <typename T, typename E>
template <typename U>
inline auto maybe::result<T, E>::map_err_value(U value) noexcept -> maybe::result<T, U>
{
    if (is_ok()) {
        return maybe::result<T, U>(std::forward<T>(ok_value()), internal::placeholder{});
    }

    return maybe::result<T, U>::err(std::forward<U>(value));
};

template <typename T, typename E>
template <typename F>
inline auto maybe::result<T, E>::and_then(F f) noexcept -> typename std::result_of<F(T)>::type
{
    typedef typename std::result_of<F(T)>::type result_t;

    if (is_err()) {
        return maybe::result<typename result_t::ok_type, E>::err(std::forward<E>(err_value()));
    }
    return f(ok_value());
};

template <typename T, typename E>
template <typename U>
inline auto maybe::result<T, E>::into_err() noexcept -> maybe::result<U, E> const
{
    if (is_err()) {
        return maybe::result<U, E>(internal::placeholder{}, std::forward<E>(err_value()));
    }
    return maybe::result<U, E>::default_ok();
};

template <typename E>
template <typename F>
inline auto maybe::result<void, E>::map(F f) noexcept
    -> maybe::result<typename std::result_of<F()>::type, E>
{
    typedef maybe::result<typename std::result_of<F()>::type, E> return_result_t;

    if (is_err()) {
        return return_result_t::err(std::forward<E>(err_value()));
    }
    return return_result_t(f(), internal::placeholder{});
};

template <typename E>
template <typename U>
inline auto maybe::result<void, E>::map_value(U value) noexcept -> maybe::result<U, E>
{
    if (is_err()) {
        return maybe::result<U, E>::err(std::forward<E>(err_value()));
    }

    return maybe::result<U, E>(std::forward<U>(value), internal::placeholder{});
};

template <typename E>
template <typename F>
inline auto maybe::result<void, E>::map_err(F f) noexcept
    -> maybe::result<void, typename std::result_of<F(E)>::type>
{
    typedef maybe::result<void, typename std::result_of<F(E)>::type> return_result_t;

    if (is_ok()) {
        return return_result_t::ok();
    }
    return return_result_t(internal::placeholder{}, f(err_value()));
};

template <typename E>
template <typename U>
inline auto maybe::result<void, E>::map_err_value(U value) noexcept -> maybe::result<void, U>
{
    if (is_ok()) {
        return maybe::result<void, U>::ok();
    }

    return maybe::result<void, U>(std::forward<U>(value));
};

template <typename E>
template <typename F>
inline auto maybe::result<void, E>::and_then(F f) noexcept -> typename std::result_of<F()>::type
{
    typedef typename std::result_of<F()>::type result_t;

    if (is_err()) {
        return maybe::result<typename result_t::ok_type, E>::err(std::forward<E>(err_value()));
    }
    return f();
};

template <typename E>
inline auto maybe::result<void, E>::into_err() noexcept -> maybe::result<void, E>
{
    if (is_err()) {
        return maybe::result<void, E>(std::forward<E>(err_value()));
    }
    return maybe::result<void, E>::default_ok();
};
