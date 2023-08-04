#pragma once

#include <cstddef>
#include <cstdint>

namespace algos {
    int naive(const char *input, size_t) noexcept;
    int naiveLessBranches(const char *input, size_t) noexcept;
    int naiveTableChar(const char *input, size_t s) noexcept;
    int naiveTableInt(const char *input, size_t s) noexcept;

    int autoVec_32(const char *i, size_t s) noexcept;
    int autoVec_64(const char *i, size_t s) noexcept;
    int autoVec_128(const char *i, size_t s) noexcept;
    int autoVec_128_IntStepCounter(const char *i, size_t s) noexcept;
    int autoVec_128_WithOverflow(const char *i, size_t s) noexcept;
    int autoVec_256(const char *i, size_t s) noexcept;
    int autoVec_256_IntStepCounter(const char *i, size_t s) noexcept;
    int autoVec_512(const char *i, size_t s) noexcept;
    int autoVec_1024(const char *i, size_t s) noexcept;
    int autoVec_2048(const char *i, size_t s) noexcept;

    int manualVec_32(const char *input, size_t s) noexcept;
    int manualVec_64(const char *input, size_t s) noexcept;
    int manualVec_128(const char *input, size_t s) noexcept;
    int manualVec_256(const char *input, size_t s) noexcept;
    int manualVec_512(const char *input, size_t s) noexcept;
    int manualVec_1024(const char *input, size_t s) noexcept;
    int manualVec_2048(const char *input, size_t s) noexcept;

    int manualVecSize_32(const char *input, size_t s) noexcept;
    int manualVecSize_64(const char *input, size_t s) noexcept;
    int manualVecSize_128(const char *input, size_t s) noexcept;
    int manualVecSize_256(const char *input, size_t s) noexcept;
    int manualVecSize_512(const char *input, size_t s) noexcept;
    int manualVecSize_1024(const char *input, size_t s) noexcept;
    int manualVecSize_2048(const char *input, size_t s) noexcept;

    int manualVecStrlen_32(const char *input, size_t) noexcept;
    int manualVecStrlen_64(const char *input, size_t) noexcept;
    int manualVecStrlen_128(const char *input, size_t) noexcept;
    int manualVecStrlen_256(const char *input, size_t) noexcept;
    int manualVecStrlen_512(const char *input, size_t) noexcept;
    int manualVecStrlen_1024(const char *input, size_t) noexcept;
    int manualVecStrlen_2048(const char *input, size_t) noexcept;
}