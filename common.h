#pragma once

#include <cstdint>
#include <stddef.h>
#include <type_traits>
#include <utility>

#if !defined(__x86_64__)
#error "The project is platform specific"
#endif

template <size_t Alignment>
inline __attribute__((always_inline)) const char* alignedAfter(const char* ptr) noexcept {
    auto offset = reinterpret_cast<std::uintptr_t>(ptr) & (Alignment - 1);
    return offset ? (ptr + Alignment - offset) : ptr;
}

template <size_t Alignment>
inline __attribute__((always_inline)) const char* alignedBefore(const char* ptr) noexcept {
    auto offset = reinterpret_cast<std::uintptr_t>(ptr) & (Alignment - 1);
    return offset ? (ptr - offset) : ptr;
}

template <size_t V>
using IndexConstant = std::integral_constant<size_t, V>;

namespace detail {
    template <class F, size_t... I>
    constexpr __attribute__((always_inline)) void repeatBlock(std::index_sequence<I...>, F&& f) {
        (f(IndexConstant<I>{}), ...);
    }
}

template <size_t N, class F>
constexpr __attribute__((always_inline)) void forEach(F&& f) {
    detail::repeatBlock(std::make_index_sequence<N>{}, std::forward<F>(f));
}

constexpr int32_t PageSize = 4096;
