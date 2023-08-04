#define CATCH_CONFIG_ENABLE_BENCHMARKING
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "util.h"
#include "algos.h"

#include <iostream>

TEST_CASE("BaseTest") {
    SECTION("Fragment") {
        constexpr auto filename = "wp_fragment.txt";
        constexpr auto expected_result = 62;

        const auto input = loadFile(filename);
        REQUIRE(strlen(input.c_str()) == input.size());

        const auto res = algos::TEST_ALGO(input.c_str(), input.size());
        REQUIRE(res == expected_result);
    }

    SECTION("AllAsses") {
        std::string input(4096, 's');
        const auto res = algos::TEST_ALGO(input.c_str(), input.size());
        REQUIRE(res == 4096);
    }

    SECTION("AllPees") {
        std::string input(4096, 'p');
        const auto res = algos::TEST_ALGO(input.c_str(), input.size());
        REQUIRE(res == -4096);
    }

    SECTION("NoneOfAssesOrPees") {
        std::string input(4096, 'a');
        const auto res = algos::TEST_ALGO(input.c_str(), input.size());
        REQUIRE(res == 0);
    }
};

TEST_CASE("CountWarAndPeace") {
    constexpr auto filename = "wp.txt";
    constexpr auto expected_size = 3359372;
    constexpr auto expected_result = 120897;

    const auto input = loadFile(filename);
    REQUIRE(input.size() == expected_size);
    REQUIRE(strlen(input.c_str()) == input.size());

    const auto res = algos::TEST_ALGO(input.c_str(), input.size());
    REQUIRE(res == expected_result);

    std::cerr << "input_file=" << filename
        << ", result=" << res
        << ", input_size=" << input.size()
        << std::endl;

    BENCHMARK("Count") {
        return algos::TEST_ALGO(input.c_str(), input.size());
    };

    BENCHMARK("StdStrLen") {
        return strlen(input.c_str());
    };
}