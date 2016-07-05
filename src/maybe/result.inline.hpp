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
    using maybe::internal::Value;

    switch (other.tag) {
        case Value::OK:
            init_ok(*other.ok_dataptr());
            break;
        case Value::ERR:
            init_err(*other.err_dataptr());
            break;
        default:
            tag = Value::NONE;
            break;
    }
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::set_from(result<T, E>& other) noexcept
{
    using maybe::internal::Value;

    switch (other.tag) {
        case Value::OK:
            init_ok(std::move(other.ok_val));
            break;
        case Value::ERR:
            init_err(std::move(other.err_val));
            break;
        default:
            tag = Value::NONE;
            break;
    }
    other.tag = Value::NONE;
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::init_ok(T&& value) noexcept
{
    tag = maybe::internal::Value::OK;
    ::new (static_cast<void*>(ok_dataptr())) T(std::forward<T>(value));
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::init_err(T&& value) noexcept
{
    tag = maybe::internal::Value::ERR;
    ::new (static_cast<void*>(err_dataptr())) T(std::forward<T>(value));
}

template <typename T, typename E>
constexpr void maybe::result<T, E>::clear() noexcept
{
    using maybe::internal::Value;

    switch (tag) {
        case Value::OK:
            (&this->ok_val)->T::~T();
            tag = Value::NONE;
            break;
        case internal::Value::ERR:
            (&this->err_val)->E::~E();
            tag = Value::NONE;
            break;
        default:
            break;
    }
}