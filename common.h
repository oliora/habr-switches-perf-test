#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <utility>

#if !defined(__x86_64__)
#error "The project is platform specific"
#endif

template <std::size_t Alignment>
inline __attribute__((always_inline)) const char* alignedAfter(const char* ptr) noexcept {
    auto offset = reinterpret_cast<std::uintptr_t>(ptr) & (Alignment - 1);
    return offset ? (ptr + Alignment - offset) : ptr;
}

template <std::size_t Alignment>
inline __attribute__((always_inline)) const char* alignedBefore(const char* ptr) noexcept {
    auto offset = reinterpret_cast<std::uintptr_t>(ptr) & (Alignment - 1);
    return offset ? (ptr - offset) : ptr;
}

template <std::size_t V>
using IndexConstant = std::integral_constant<std::size_t, V>;

namespace detail {
    template <class F, std::size_t... I>
    constexpr __attribute__((always_inline)) void repeatBlock(std::index_sequence<I...>, F&& f) {
        (f(IndexConstant<I>{}), ...);
    }
}

// Call `f` `N` times as `(f(IndexConstant<0>{}), f(IndexConstant<1>{}), ... f(IndexConstant<N-1>{}))`
template <std::size_t N, class F>
constexpr __attribute__((always_inline)) void forEach(F&& f) {
    detail::repeatBlock(std::make_index_sequence<N>{}, std::forward<F>(f));
}

constexpr int32_t PageSize = 4096;
