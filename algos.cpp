#include "algos.h"
#include "common.h"
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string_view>
#include <utility>
#include <limits>
#include <immintrin.h>

namespace algos {

int naive(const char* input, std::size_t) noexcept {
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

int naiveLessBranches(const char* input, std::size_t) noexcept {
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
inline int naiveTable(const char* input, std::size_t) noexcept {
    int res = 0;
    while (true) {
        auto c = *input++;
        if (!c) [[unlikely]] {
            return res;
        }
        res += CharValueTable<T>::sTable[static_cast<unsigned char>(c)];
    }
}

int naiveTableChar(const char* input, std::size_t s) noexcept {
    return naiveTable<signed char>(input, s);
}

int naiveTableInt(const char* input, std::size_t s) noexcept {
    return naiveTable<int>(input, s);
}

/////////////////////////////////////////////////////////////////

#define step_size 64

#define proc(v) ({ auto _v = (v); (_v == 's') - (_v == 'p'); })

#define aligned(ptr, align) ({ \
    auto _ptr = (ptr); \
    auto _align = (align); \
    auto _r = (long long)_ptr & (_align - 1); \
    _r ? _ptr + (_align - _r) : _ptr; \
})

inline int autoVec_64_Orig_(const unsigned char* i) noexcept {
    auto r = 0;
    for (auto head_end = aligned(i, step_size); i != head_end && *i; ++i) {
        r += proc(*i);
    }
    if (!*i) {
        return r;
    }
    while (1) {
        signed char step_r = 0;
        unsigned char rs[step_size];
        for (auto n = 0; n != sizeof(rs); ++n) {
            rs[n] = i[n] ? 0 : ~0;
            step_r += proc(i[n]);
        }
        auto _ = reinterpret_cast<const long long*>(rs);
        if (_[0] | _[1] | _[2] | _[3] | _[4] | _[5] | _[6] | _[7]) {
            while (*i) r += proc(*i++);
            break;
        }
        r += step_r;
        i += sizeof(rs);
    }
    return r;
}

int autoVec_64_Orig(const char* input, std::size_t) noexcept {
    return autoVec_64_Orig_(reinterpret_cast<const unsigned char*>(input));
}

/////////////////////////////////////////////////////////////////

template <std::size_t... I>
constexpr __attribute__((always_inline)) long long bitwiseOr(const long long* ptr, std::index_sequence<I...>) noexcept {
    return (ptr[I] | ...);
}

template <std::size_t StepSize>
using StepResultType = std::conditional_t<StepSize <= std::numeric_limits<signed char>::max(), signed char, short>;

template <std::size_t StepSize, std::signed_integral StepResultT = StepResultType<StepSize>>
requires((StepSize <= PageSize)
    && (PageSize % StepSize == 0)
    && (StepSize % sizeof(long long) == 0)
    //&& (StepSize <= std::numeric_limits<StepResultT>::max())
    //&& (-StepSize >= std::numeric_limits<StepResultT>::min())
    )
inline int autoVec(const char* input, std::size_t) noexcept {
    constexpr std::size_t BlocksPerStep = StepSize / sizeof(long long);

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

        if (bitwiseOr(reinterpret_cast<const long long*>(nulls), std::make_index_sequence<BlocksPerStep>{})) [[unlikely]] {
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

int autoVec_32(const char* i, std::size_t s) noexcept {
    return autoVec<32>(i, s);
}

int autoVec_64(const char* i, std::size_t s) noexcept {
    return autoVec<64>(i, s);
}

int autoVec_128(const char* i, std::size_t s) noexcept {
    return autoVec<128>(i, s);
}

int autoVec_128_IntStepCounter(const char* i, std::size_t s) noexcept {
    return autoVec<128, int>(i, s);
}

int autoVec_128_WithOverflow(const char* i, std::size_t s) noexcept {
    return autoVec<128, signed char>(i, s);
}

int autoVec_256(const char* i, std::size_t s) noexcept {
    return autoVec<256>(i, s);
}

int autoVec_256_IntStepCounter(const char* i, std::size_t s) noexcept {
    return autoVec<256, int>(i, s);
}

int autoVec_512(const char* i, std::size_t s) noexcept {
    return autoVec<512>(i, s);
}

int autoVec_1024(const char* i, std::size_t s) noexcept {
    return autoVec<1024>(i, s);
}

int autoVec_2048(const char* i, std::size_t s) noexcept {
    return autoVec<2048>(i, s);
}

/////////////////////////////////////////////////////////////////

int manualVec_64_Orig(const char* input, std::size_t) noexcept {
    int res = 0;

    // Process unaligned preamble in naive way
    for (const auto headEnd = alignedAfter<64>(input); input != headEnd && *input; ++input) {
        res += charValue(*input);
    }
    if (!*input) [[unlikely]] {
        return res;
    }

    const __m256i nulls = _mm256_set1_epi8(0); // Fill all 32 8-bit integers with 0
    const __m256i ps = _mm256_set1_epi8('p'); // Fill all 32 8-bit integers with 'p'
    const __m256i ss = _mm256_set1_epi8('s'); // Fill all 32 8-bit integers with 's'

    while (true) {
        // We use aligned load because we know that the input is aligned
        const __m256i block0 = _mm256_load_si256(reinterpret_cast<const __m256i*>(input));
        const __m256i block1 = _mm256_load_si256(reinterpret_cast<const __m256i*>(input + 32));

        __m256i eqNull = _mm256_cmpeq_epi8(nulls, block0);
        eqNull = _mm256_cmpeq_epi8(eqNull, block1);

        __m256i counters = _mm256_sub_epi8(_mm256_cmpeq_epi8(ps, block0), _mm256_cmpeq_epi8(ss, block0));
        counters = _mm256_add_epi8(counters, _mm256_cmpeq_epi8(ps, block1));
        counters = _mm256_sub_epi8(counters, _mm256_cmpeq_epi8(ss, block1));

        // Horizontally sum each consequitive 8 absolute differences (abs(counters[i] - nulls[i])) and put results into four 16-bit integers
        // that is an intrinsic for `vpsadbw` instruction that I've mentioned in the above `autoVec_64` assembly comments
        counters = _mm256_sad_epu8(counters, nulls);

        __m128i stepRes = _mm_add_epi64(_mm256_extracti128_si256(counters, 1), *reinterpret_cast<const __m128i*>(&counters));
        stepRes = _mm_add_epi64(_mm_bsrli_si128(stepRes, 8), stepRes);

        // Fold `eqNull` to a 64-bit integer with birwise or
        __m128i stepNull = _mm_or_si128(_mm256_extracti128_si256(eqNull, 1), *reinterpret_cast<const __m128i*>(&eqNull));
        stepNull = _mm_or_si128(_mm_bsrli_si128(stepNull, 8), stepNull);
        const auto anyNull = _mm_extract_epi64(stepNull, 0);
        if (anyNull) [[unlikely]] {
            // Found null character, calculate the remaining values char by char until we hit the null character
            while (*input) {
                res += charValue(*input++);
            }
            return res;
        }

        // Take the lowest byte of the lowest 64-bit integer `stepRes` and convert it to a signed char
        res += static_cast<signed char>(stepRes[0]);
        input += 64;
    }
}

/////////////////////////////////////////////////////////////////

template <std::size_t StepSize>
requires((StepSize <= PageSize)
    && (PageSize % StepSize == 0)
    && (StepSize % sizeof(long long) == 0))
inline int manualVec(const char* input, std::size_t) noexcept {
    constexpr std::size_t BlocksPerStep = StepSize / sizeof(__m256i);

    int res = 0;

    // Process unaligned preamble in naive way
    for (const auto headEnd = alignedAfter<64>(input); input != headEnd && *input; ++input) {
        res += charValue(*input);
    }
    if (!*input) [[unlikely]] {
        return res;
    }

    const __m256i nulls = _mm256_set1_epi8(0); // Fill all packed 8-bit integers with 0
    const __m256i ps = _mm256_set1_epi8('p');  // Fill all packed 8-bit integers with 'p'
    const __m256i ss = _mm256_set1_epi8('s');  // Fill all packed 8-bit integers with 's'

    // Offset to add to counters to make them non-negative
    const __m256i vecOffset = _mm256_set1_epi8(BlocksPerStep);
    constexpr int scalarOffset = BlocksPerStep * sizeof(__m256i);

    __m256i resVec{};

    while (true) {
        __m256i eqNull;
        __m256i counters;
        forEach<BlocksPerStep>([&]<std::size_t Idx>(IndexConstant<Idx>) {
            const __m256i block = _mm256_load_si256(reinterpret_cast<const __m256i*>(input) + Idx);

            if constexpr (Idx == 0) {
                eqNull = _mm256_cmpeq_epi8(nulls, block);
                counters = _mm256_sub_epi8(_mm256_cmpeq_epi8(ps, block), _mm256_cmpeq_epi8(ss, block));
            } else {
                eqNull = _mm256_or_si256(eqNull, _mm256_cmpeq_epi8(nulls, block));
                counters = _mm256_add_epi8(counters, _mm256_cmpeq_epi8(ps, block));
                counters = _mm256_sub_epi8(counters, _mm256_cmpeq_epi8(ss, block));
            }
        });

        // Each 8-bit counter in `counters` fits into signed char, effectively works for BlocksPerStep < 128 hence max StepSize is 2048
        static_assert(BlocksPerStep < 128);
        // Offset 8-bit counters to be non-negative
        counters = _mm256_add_epi8(counters, vecOffset);
        // Horisontally sum 8-bit counters in groups by 8 to produce 4 16-bit counters
        counters = _mm256_sad_epu8(counters, nulls);

        // Fold `eqNull` to a 64-bit integer with birwise or
        __m128i stepNull = _mm_or_si128(_mm256_extracti128_si256(eqNull, 1), *reinterpret_cast<const __m128i*>(&eqNull));
        stepNull = _mm_or_si128(_mm_bsrli_si128(stepNull, 8), stepNull);
        const auto anyNull = _mm_extract_epi64(stepNull, 0);
        if (anyNull) [[unlikely]] {
            // Found null character, calculate the remaining values char by char until we hit the null character
            while (*input) {
                res += charValue(*input++);
            }

            // Fold 4 64-bit counters in resVec to a 64-bit integer with addition
            __m128i stepRes = _mm_add_epi64(_mm256_extracti128_si256(resVec, 1), *reinterpret_cast<const __m128i*>(&resVec));
            stepRes = _mm_add_epi64(_mm_bsrli_si128(stepRes, 8), stepRes);
            res += static_cast<int>(_mm_extract_epi64(stepRes, 0));
            return res;
        }

        // Sum them with 4 64-bit counters in resVec
        resVec = _mm256_add_epi64(resVec, counters);

        // Compensate for added vecOffset
        res -= scalarOffset;

        input += StepSize;
    }
}

int manualVec_32(const char* input, std::size_t s) noexcept {
    return manualVec<32>(input, s);
}

int manualVec_64(const char* input, std::size_t s) noexcept {
    return manualVec<64>(input, s);
}

int manualVec_128(const char* input, std::size_t s) noexcept {
    return manualVec<128>(input, s);
}

int manualVec_256(const char* input, std::size_t s) noexcept {
    return manualVec<256>(input, s);
}

int manualVec_512(const char* input, std::size_t s) noexcept {
    return manualVec<512>(input, s);
}

int manualVec_1024(const char* input, std::size_t s) noexcept {
    return manualVec<1024>(input, s);
}

int manualVec_2048(const char* input, std::size_t s) noexcept {
    return manualVec<2048>(input, s);
}

/////////////////////////////////////////////////////////////////

template <std::size_t StepSize>
requires((StepSize <= PageSize)
    && (PageSize % StepSize == 0)
    && (StepSize % sizeof(long long) == 0))
inline int manualVecSize(const char* input, std::size_t size) noexcept {
    constexpr std::size_t BlocksPerStep = StepSize / sizeof(__m256i);

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

        const __m256i nulls = _mm256_set1_epi8(0); // Fill all packed 8-bit integers with 0
        const __m256i ps = _mm256_set1_epi8('p');  // Fill all packed 8-bit integers with 'p'
        const __m256i ss = _mm256_set1_epi8('s');  // Fill all packed 8-bit integers with 's'

        // Offset to add to step counters to make them non-negative
        const __m256i vecOffset = _mm256_set1_epi8(BlocksPerStep);
        constexpr int scalarOffset = BlocksPerStep * sizeof(__m256i);

        __m256i resVec{};

        while (input != alignedInputEnd) {
            __m256i counters;
            forEach<BlocksPerStep>([&]<std::size_t Idx>(IndexConstant<Idx>) {
                const __m256i block = _mm256_load_si256(reinterpret_cast<const __m256i*>(input) + Idx);

                if constexpr (Idx == 0) {
                    counters = _mm256_sub_epi8(_mm256_cmpeq_epi8(ps, block), _mm256_cmpeq_epi8(ss, block));
                } else {
                    counters = _mm256_add_epi8(counters, _mm256_cmpeq_epi8(ps, block));
                    counters = _mm256_sub_epi8(counters, _mm256_cmpeq_epi8(ss, block));
                }
            });

            // Each 8-bit counter in `counters` fits into signed char, effectively works for BlocksPerStep < 128 hence max StepSize is 2048
            static_assert(BlocksPerStep < 128);
            // Offset 8-bit counters to be non-negative
            counters = _mm256_add_epi8(counters, vecOffset);
            // Horisontally sum 8-bit counters in groups by 8 to produce 4 16-bit counters
            counters = _mm256_sad_epu8(counters, nulls);
            // Sum them with 4 64-bit counters in resVec
            resVec = _mm256_add_epi64(resVec, counters);

            // Compensate for added `vecOffset`
            res -= scalarOffset;

            input += StepSize;
        }

        // Fold 4 64-bit counters in resVec to a 64-bit integer with addition
        __m128i stepRes = _mm_add_epi64(_mm256_extracti128_si256(resVec, 1), *reinterpret_cast<const __m128i*>(&resVec));
        stepRes = _mm_add_epi64(_mm_bsrli_si128(stepRes, 8), stepRes);
        res += static_cast<int>(_mm_extract_epi64(stepRes, 0));
    }

    // TODO: To minimize slow naive part finish up with 32 byte steps before going naive

    // Process final part
    for ( ; input != inputEnd; ++input) {
        res += charValue(*input);
    }

    return res;
}

int manualVecSize_32(const char* input, std::size_t s) noexcept {
    return manualVecSize<32>(input, s);
}

int manualVecSize_64(const char* input, std::size_t s) noexcept {
    return manualVecSize<64>(input, s);
}

int manualVecSize_128(const char* input, std::size_t s) noexcept {
    return manualVecSize<128>(input, s);
}

int manualVecSize_256(const char* input, std::size_t s) noexcept {
    return manualVecSize<256>(input, s);
}

int manualVecSize_512(const char* input, std::size_t s) noexcept {
    return manualVecSize<512>(input, s);
}

int manualVecSize_1024(const char* input, std::size_t s) noexcept {
    return manualVecSize<1024>(input, s);
}

int manualVecSize_2048(const char* input, std::size_t s) noexcept {
    return manualVecSize<2048>(input, s);
}

/////////////////////////////////////////////////////////////////

int manualVecStrlen_32(const char* input, std::size_t) noexcept {
    return manualVecSize<32>(input, strlen(input));
}

int manualVecStrlen_64(const char* input, std::size_t) noexcept {
    return manualVecSize<64>(input, strlen(input));
}

int manualVecStrlen_128(const char* input, std::size_t) noexcept {
    return manualVecSize<128>(input, strlen(input));
}

int manualVecStrlen_256(const char* input, std::size_t) noexcept {
    return manualVecSize<256>(input, strlen(input));
}

int manualVecStrlen_512(const char* input, std::size_t) noexcept {
    return manualVecSize<512>(input, strlen(input));
}

int manualVecStrlen_1024(const char* input, std::size_t) noexcept {
    return manualVecSize<1024>(input, strlen(input));
}

int manualVecStrlen_2048(const char* input, std::size_t) noexcept {
    return manualVecSize<2048>(input, strlen(input));
}

}