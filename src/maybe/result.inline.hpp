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
template <typename F>
inline auto maybe::result<T, E>::map(F f) noexcept -> maybe::result<typename std::result_of<F(T)>::type, E>
{
    typedef maybe::result<typename std::result_of<F(T)>::type, E> return_result_t;

    if (is_err()) {
        return return_result_t::err(std::forward<E>(err_value()));
    }
    return return_result_t::ok(f(std::forward<T>(ok_value())));
};

template <typename T, typename E>
template <typename U>
inline auto maybe::result<T, E>::map_value(U value) noexcept -> maybe::result<U, E>
{
    if (is_err()) {
        return maybe::result<U, E>::err(std::forward<E>(err_value()));
    }

    return maybe::result<U, E>::ok(std::forward<U>(value));
};

template <typename T, typename E>
template <typename F>
inline auto maybe::result<T, E>::map_err(F f) noexcept
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
inline auto maybe::result<T, E>::map_err_value(U value) noexcept -> maybe::result<T, U>
{
    if (is_ok()) {
        return maybe::result<T, U>::ok(std::forward<T>(ok_value()));
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
    return f(std::forward<T>(ok_value()));
};

template <typename T, typename E>
template <typename R>
inline auto maybe::result<T, E>::into_err() noexcept -> R
{
    if (is_err()) {
        return R::err(std::forward<E>(err_value()));
    }
    return R::default_ok();
};