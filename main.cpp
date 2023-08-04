#include "util.h"
#include "algos.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <type_traits>


int main(int argc, const char **argv) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [<num_runs = 1>]\n";
        return 1;
    }

    int numRuns = 1;
    if (argc > 2) {
        numRuns = atoi(argv[2]);
        if (numRuns == 0) {
            std::cerr << "Usage: " << argv[0] << " <input_file> [<num_runs = 1>]\n";
            return 1;
        }
    }

    const auto filename = argv[1];
    const auto content = loadFile(filename);
    clobberMemory();

    std::chrono::nanoseconds minDuration = std::chrono::nanoseconds::max();
    int resultValue = 0;

    for (int i = 0; i != numRuns; ++i) {
        const auto start = PerfClock::now();
        resultValue = algos::TEST_ALGO(content.c_str(), content.size());
        const auto duration = std::chrono::nanoseconds(PerfClock::now() - start);
        minDuration = std::min(minDuration, duration);
    }
    clobberMemory();

    const auto maxSpeed = content.size() * NsPerS / minDuration.count() / Mebibyte;

    std::cout <<
        "{" \
            "\"algo\": \"" STR(TEST_ALGO) "\", " \
            "\"input_file\": \"" << filename << "\", " \
            "\"input_size\": " << content.size() << ", " \
            "\"num_runs\": " << numRuns << ", " \
            "\"result\": " << resultValue << ", " \
            "\"time_ns\": " << minDuration.count() << ", " \
            "\"mib_per_s\": " << maxSpeed <<
        "}\n";

    return 0;
}
