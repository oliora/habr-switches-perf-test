#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <atomic>

inline std::string loadFile(const char *path) {
    std::string res;

    std::ifstream is{path};
    if (!is) {
        throw std::runtime_error("Cannot open file " + std::string(path));
    }

    is.seekg(0, std::ios::end);
    res.resize(is.tellg());
    is.seekg(0, std::ios::beg);
    is.read(res.data(), res.size());
    return res;
}

using PerfClock = std::conditional_t<std::chrono::high_resolution_clock::is_steady,
    std::chrono::high_resolution_clock,
    std::chrono::steady_clock>;

constexpr auto NsPerS = 1'000'000'000;
constexpr auto Mebibyte = 1024 * 1024;

inline __attribute__((always_inline)) void clobberMemory() noexcept {
    std::atomic_signal_fence(std::memory_order_acq_rel);
}
