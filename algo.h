#pragma once

#include "common.h"
#include <cstdint>
#include <cstring>
#include <string_view>
#include <utility>
#include <limits>
#include <immintrin.h>

namespace algos {

__attribute__((__noinline__)) int naive(const char *input, size_t) noexcept {
    int res = 0;
    while (true) {
        auto c = *input++;
        switch (c) {
        case 0:
            return res;
        case 's':
            res++;
            break;
        case 'p':
            res--;
            break;
        default:
            break;
        }
    }
}

/////////////////////////////////////////////////////////////////

constexpr __attribute__((always_inline)) int charValue(char c) noexcept {
    return (c == 's') - (c == 'p');
}

__attribute__((__noinline__)) int naiveLessBranches(const char *input, size_t) noexcept {
    int res = 0;
    while (true) {
        auto c = *input++;
        if (!c) [[unlikely]] {
            return res;
        }
        res += charValue(c);
    }
}

/////////////////////////////////////////////////////////////////

template <std::integral T>
struct CharValueTable {
    static constexpr T sTable[256] = {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        -1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    };
};

template <std::integral T>
inline int naiveTable(const char *input, size_t) noexcept {
    int res = 0;
    while (true) {
        auto c = *input++;
        if (!c) [[unlikely]] {
            return res;
        }
        res += CharValueTable<T>::sTable[static_cast<unsigned char>(c)];
    }
}

__attribute__((__noinline__)) int naiveTableChar(const char *input, size_t s) noexcept {
    return naiveTable<signed char>(input, s);
}

__attribute__((__noinline__)) int naiveTableInt(const char *input, size_t s) noexcept {
    return naiveTable<int>(input, s);
}

/////////////////////////////////////////////////////////////////

template <size_t... I>
constexpr __attribute__((always_inline)) long long bitwiseOr(const long long* ptr, std::index_sequence<I...>) noexcept {
    return (ptr[I] | ...);
}

template <size_t StepSize>
using StepResultType = std::conditional_t<StepSize <= std::numeric_limits<signed char>::max(), signed char, short>;

template <size_t StepSize, std::signed_integral StepResultT = StepResultType<StepSize>>
requires((StepSize <= PageSize)
    && (PageSize % StepSize == 0)
    && (StepSize % sizeof(long long) == 0)
    //&& (StepSize <= std::numeric_limits<StepResultT>::max())
    //&& (-StepSize >= std::numeric_limits<StepResultT>::min())
    )
inline int autoVec(const char *input, size_t) noexcept {
    constexpr size_t BlocksPerStep = StepSize / sizeof(long long);

    int res = 0;

    // Process unaligned preamble in naive way
    {
        const auto headEnd = alignedAfter<StepSize>(input);
        while (input != headEnd && *input) {
            res += charValue(*input++);
        }
        if (input != headEnd) [[unlikely]] {
            return res;
        }
    }

    // Process the remaining alignedAfter part. Note that there is UB in reading past the end of the buffer but it is fine on x86 platform as far as we
    // only read from the same memory page (which we do)
    while (true) {
        unsigned char nulls[StepSize];
        StepResultT step_res = 0;
        for (auto i = 0; i != sizeof(nulls); ++i) {
            char c = input[i];
            nulls[i] = c ? 0 : ~0;
            step_res += charValue(c);
        }

        long long anyNull = 0;
        for (auto i = 0; i != BlocksPerStep; ++i) {
            anyNull |= reinterpret_cast<const long long*>(nulls)[i];
        }

        //if (bitwiseOr(reinterpret_cast<const long long*>(nulls), std::make_index_sequence<BlocksPerStep>{})) [[unlikely]] {
        if (anyNull) [[unlikely]] {
            // Found null character, calculate the remaining values char by char untill we hit the null character
            while (*input) {
                res += charValue(*input++);
            }
            return res;
        }
        res += step_res;
        input += sizeof(nulls);
    }
}

__attribute__((__noinline__)) int autoVec_32(const char *i, size_t s) noexcept {
    return autoVec<32>(i, s);
}

__attribute__((__noinline__)) int autoVec_64(const char *i, size_t s) noexcept {
    return autoVec<64>(i, s);
}

__attribute__((__noinline__)) int autoVec_128(const char *i, size_t s) noexcept {
    return autoVec<128>(i, s);
}

__attribute__((__noinline__)) int autoVec_128_IntStepCounter(const char *i, size_t s) noexcept {
    return autoVec<128, int>(i, s);
}

__attribute__((__noinline__)) int autoVec_128_WithOverflow(const char *i, size_t s) noexcept {
    return autoVec<128, signed char>(i, s);
}

__attribute__((__noinline__)) int autoVec_256(const char *i, size_t s) noexcept {
    return autoVec<256>(i, s);
}

__attribute__((__noinline__)) int autoVec_256_IntStepCounter(const char *i, size_t s) noexcept {
    return autoVec<256, int>(i, s);
}

__attribute__((__noinline__)) int autoVec_512(const char *i, size_t s) noexcept {
    return autoVec<512>(i, s);
}

__attribute__((__noinline__)) int autoVec_1024(const char *i, size_t s) noexcept {
    return autoVec<1024>(i, s);
}

__attribute__((__noinline__)) int autoVec_2048(const char *i, size_t s) noexcept {
    return autoVec<2048>(i, s);
}

/////////////////////////////////////////////////////////////////

template <size_t StepSize>
requires((StepSize <= PageSize)
    && (PageSize % StepSize == 0)
    && (StepSize % sizeof(long long) == 0))
inline int manualVec(const char *input, size_t) noexcept {
    constexpr size_t BlocksPerStep = StepSize / sizeof(__m256i);

    int res = 0;

    // Process unaligned preamble in naive way
    {
        const auto headEnd = alignedAfter<StepSize>(input);
        while (input != headEnd && *input) {
            res += charValue(*input++);
        }
        if (input != headEnd) [[unlikely]] {
            return res;
        }
    }

    const __m256i null_c = _mm256_set1_epi8(0); // Fill all packed 8-bit integers with 0
    const __m256i p_c = _mm256_set1_epi8('p');  // Fill all packed 8-bit integers with 'p'
    const __m256i s_c = _mm256_set1_epi8('s');  // Fill all packed 8-bit integers with 's'

    const __m256i posOffset = _mm256_set1_epi8(BlocksPerStep); // Offset to add to counters to make them non-negative
    constexpr int negOffset = BlocksPerStep * sizeof(__m256i);
    __m256i resVec{};

    while (true) {
        __m256i eq_null;
        __m256i counters;
        forEach<BlocksPerStep>([&]<size_t Idx>(IndexConstant<Idx>) {
            const __m256i block = _mm256_load_si256(reinterpret_cast<const __m256i*>(input) + Idx);

            if constexpr (Idx == 0) {
                eq_null = _mm256_cmpeq_epi8(null_c, block);
                counters = _mm256_sub_epi8(_mm256_cmpeq_epi8(p_c, block), _mm256_cmpeq_epi8(s_c, block));
            } else {
                eq_null = _mm256_or_si256(eq_null, _mm256_cmpeq_epi8(null_c, block));
                counters = _mm256_add_epi8(counters, _mm256_cmpeq_epi8(p_c, block));
                counters = _mm256_sub_epi8(counters, _mm256_cmpeq_epi8(s_c, block));
            }
        });

        // Fold 8 byte words of `counters` by OR-ing their packed 8-bit ints
        __m128i step_null = _mm_or_si128(_mm256_extracti128_si256(eq_null, 1), *reinterpret_cast<const __m128i*>(&eq_null));
        step_null = _mm_or_si128(_mm_bsrli_si128(step_null, 8), step_null);
        const auto anyNull = _mm_extract_epi64(step_null, 0);
        if (anyNull) [[unlikely]] {
            // Found null character, calculate the remaining values char by char until we hit the null character
            while (*input) {
                res += charValue(*input++);
            }

            // Sum up vector counters
            __m128i step_res = _mm_add_epi64(_mm256_extracti128_si256(resVec, 1), *reinterpret_cast<const __m128i*>(&resVec));
            step_res = _mm_add_epi64(_mm_bsrli_si128(step_res, 8), step_res);
            res += static_cast<int>(_mm_extract_epi64(step_res, 0));
            return res;
        }

        // Each 8-bit counter of `counters` fits into signed char, effectively works for BlocksPerStep <= 64 (hence StepSize <= 2048)
        static_assert(BlocksPerStep < 128);
        // Make 8-bit counters to be non-negative
        counters = _mm256_add_epi8(counters, posOffset);
        // Sum up 32 8-bit counters to produce 4 16-bit counters
        counters = _mm256_sad_epu8(counters, null_c);
        resVec = _mm256_add_epi64(resVec, counters);

        // Compensate for added offset
        res -= negOffset;

        input += StepSize;
    }
}

__attribute__((__noinline__)) int manualVec_32(const char *input, size_t s) noexcept {
    return manualVec<32>(input, s);
}

__attribute__((__noinline__)) int manualVec_64(const char *input, size_t s) noexcept {
    return manualVec<64>(input, s);
}

__attribute__((__noinline__)) int manualVec_128(const char *input, size_t s) noexcept {
    return manualVec<128>(input, s);
}

__attribute__((__noinline__)) int manualVec_256(const char *input, size_t s) noexcept {
    return manualVec<256>(input, s);
}

__attribute__((__noinline__)) int manualVec_512(const char *input, size_t s) noexcept {
    return manualVec<512>(input, s);
}

__attribute__((__noinline__)) int manualVec_1024(const char *input, size_t s) noexcept {
    return manualVec<1024>(input, s);
}

__attribute__((__noinline__)) int manualVec_2048(const char *input, size_t s) noexcept {
    return manualVec<2048>(input, s);
}

/////////////////////////////////////////////////////////////////

template <size_t StepSize, bool AlignedLoad = true>
requires((StepSize <= PageSize)
    && (PageSize % StepSize == 0)
    && (StepSize % sizeof(long long) == 0))
inline int manualVecSize(const char *input, size_t size) noexcept {
    constexpr size_t BlocksPerStep = StepSize / sizeof(__m256i);

    int res = 0;
    const auto inputEnd = input + size;

    // TODO: To minimize slow naive part rump up with with 32 byte steps until StepSize alignment is reached

    // Process unaligned preamble in naive way
    for (const auto headEnd = std::min(inputEnd, alignedAfter<StepSize>(input)); input != headEnd; ++input) {
        res += charValue(*input);
    }
    if (input == inputEnd) [[unlikely]] {
        return res;
    }

    // Process aligned part in StepSize without reading past the buffer
    {
        const auto alignedInputEnd = alignedBefore<StepSize>(inputEnd);

        const __m256i null_c = _mm256_set1_epi8(0); // Fill all packed 8-bit integers with 0
        const __m256i p_c = _mm256_set1_epi8('p');  // Fill all packed 8-bit integers with 'p'
        const __m256i s_c = _mm256_set1_epi8('s');  // Fill all packed 8-bit integers with 's'

        // Offset to add to step counters to make them non-negative
        const __m256i posOffset = _mm256_set1_epi8(BlocksPerStep);
        constexpr int negOffset = BlocksPerStep * sizeof(__m256i);

        __m256i resVec{};

        while (input != alignedInputEnd) {
            __m256i counters;
            forEach<BlocksPerStep>([&]<size_t Idx>(IndexConstant<Idx>) {
                __m256i block;
                if constexpr (AlignedLoad) {
                    block = _mm256_load_si256(reinterpret_cast<const __m256i*>(input) + Idx);
                } else {
                    block = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(input) + Idx);
                }

                if constexpr (Idx == 0) {
                    counters = _mm256_sub_epi8(_mm256_cmpeq_epi8(p_c, block), _mm256_cmpeq_epi8(s_c, block));
                } else {
                    counters = _mm256_add_epi8(counters, _mm256_cmpeq_epi8(p_c, block));
                    counters = _mm256_sub_epi8(counters, _mm256_cmpeq_epi8(s_c, block));
                }
            });

            if constexpr (StepSize < 128) {
                // Total step counter fits into signed char, effectively works for StepSize <= 64
                // Fold 8 byte words of `counters` by adding their packed 8-bit ints
                __m128i step_res = _mm_add_epi8(_mm256_extracti128_si256(counters, 1), *reinterpret_cast<const __m128i*>(&counters));
                step_res = _mm_add_epi8(_mm_bsrli_si128(step_res, 8), step_res);
                // Compute the absolute differences of packed unsigned 8-bit integers in step_res and null_c
                // This works for us thx to unsigned arithmetic
                step_res = _mm_sad_epu8(step_res, *reinterpret_cast<const __m128i*>(&null_c));
                // Get the lowest byte of step_res as signed char
                res += static_cast<signed char>(_mm_extract_epi8(step_res, 0));
            } else {
                // Each 8-bit counter of `counters` fits into signed char, effectively works for BlocksPerStep <= 64 (hence StepSize <= 2048)
                static_assert(BlocksPerStep < 128);
                // Make 8-bit counters to be non-negative
                counters = _mm256_add_epi8(counters, posOffset);
                // Sum up 32 8-bit counters to produce 4 16-bit counters
                counters = _mm256_sad_epu8(counters, null_c);
                resVec = _mm256_add_epi64(resVec, counters);

                // Compensate for added `posOffset`
                res -= negOffset;
            }

            input += StepSize;
        }

        if constexpr (StepSize >= 128) {
            // Sum up vector counters
            __m128i step_res = _mm_add_epi64(_mm256_extracti128_si256(resVec, 1), *reinterpret_cast<const __m128i*>(&resVec));
            step_res = _mm_add_epi64(_mm_bsrli_si128(step_res, 8), step_res);
            res += static_cast<int>(_mm_extract_epi64(step_res, 0));
        }
    }

    // TODO: To minimize slow naive part finish up with 32 byte steps before going naive

    // Process final part
    for ( ; input != inputEnd; ++input) {
        res += charValue(*input);
    }

    return res;
}

__attribute__((__noinline__)) int manualVecSize_32(const char *input, size_t s) noexcept {
    return manualVecSize<32>(input, s);
}

__attribute__((__noinline__)) int manualVecSize_64(const char *input, size_t s) noexcept {
    return manualVecSize<64>(input, s);
}

__attribute__((__noinline__)) int manualVecSize_128(const char *input, size_t s) noexcept {
    return manualVecSize<128>(input, s);
}

__attribute__((__noinline__)) int manualVecSize_256(const char *input, size_t s) noexcept {
    return manualVecSize<256>(input, s);
}

__attribute__((__noinline__)) int manualVecSize_512(const char *input, size_t s) noexcept {
    return manualVecSize<512>(input, s);
}

__attribute__((__noinline__)) int manualVecSize_1024(const char *input, size_t s) noexcept {
    return manualVecSize<1024>(input, s);
}

__attribute__((__noinline__)) int manualVecSize_2048(const char *input, size_t s) noexcept {
    return manualVecSize<2048>(input, s);
}

// /////////////////////////////////////////////////////////////////
//
// __attribute__((__noinline__)) int manualVecSizeUnaligned_32(const char *input, size_t s) noexcept {
//     return manualVecSize<32, false>(input, s);
// }
//
// __attribute__((__noinline__)) int manualVecSizeUnaligned_64(const char *input, size_t s) noexcept {
//     return manualVecSize<64, false>(input, s);
// }
//
// __attribute__((__noinline__)) int manualVecSizeUnaligned_128(const char *input, size_t s) noexcept {
//     return manualVecSize<128, false>(input, s);
// }
//
// __attribute__((__noinline__)) int manualVecSizeUnaligned_256(const char *input, size_t s) noexcept {
//     return manualVecSize<256, false>(input, s);
// }
//
// __attribute__((__noinline__)) int manualVecSizeUnaligned_512(const char *input, size_t s) noexcept {
//     return manualVecSize<512, false>(input, s);
// }
//
// __attribute__((__noinline__)) int manualVecSizeUnaligned_1024(const char *input, size_t s) noexcept {
//     return manualVecSize<1024, false>(input, s);
// }
//
// __attribute__((__noinline__)) int manualVecSizeUnaligned_2048(const char *input, size_t s) noexcept {
//     return manualVecSize<2048, false>(input, s);
// }
//

/////////////////////////////////////////////////////////////////

__attribute__((__noinline__)) int manualVecStrlen_32(const char *input, size_t) noexcept {
    return manualVecSize<32>(input, strlen(input));
}

__attribute__((__noinline__)) int manualVecStrlen_64(const char *input, size_t) noexcept {
    return manualVecSize<64>(input, strlen(input));
}

__attribute__((__noinline__)) int manualVecStrlen_128(const char *input, size_t) noexcept {
    return manualVecSize<128>(input, strlen(input));
}

__attribute__((__noinline__)) int manualVecStrlen_256(const char *input, size_t) noexcept {
    return manualVecSize<256>(input, strlen(input));
}

__attribute__((__noinline__)) int manualVecStrlen_512(const char *input, size_t) noexcept {
    return manualVecSize<512>(input, strlen(input));
}

__attribute__((__noinline__)) int manualVecStrlen_1024(const char *input, size_t) noexcept {
    return manualVecSize<1024>(input, strlen(input));
}

__attribute__((__noinline__)) int manualVecStrlen_2048(const char *input, size_t) noexcept {
    return manualVecSize<2048>(input, strlen(input));
}

}