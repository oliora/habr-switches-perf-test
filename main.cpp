#include "util.h"
#include "algo.h"

#include <atomic>
#include <chrono>
#include <cstring>
#include <type_traits>


int main(int argc, const char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    const auto filename = argv[1];
    const auto content = loadFile(filename);

//     {
//         clobberMemory();
//         const auto start = PerfClock::now();
//         auto len = strlen(content.c_str());
//         const auto duration = std::chrono::nanoseconds(PerfClock::now() - start);
//         const auto speed = len * NsPerS / duration.count() / Mebibyte;
//         clobberMemory();
//
//         std::cout << "input_file=" << filename
//             << ", strlen=" << len
//             << ", time_ns=" << duration.count()
//             << ", mib_per_s=" << speed
//             << "\n";
//
//         if (len != content.size()) {
//             std::cerr << "Input contains null character";
//             return 1;
//         }
//     }

    clobberMemory();
    const auto start = PerfClock::now();
    const auto res = algos::TEST_ALGO(content.c_str(), content.size());
    const auto duration = std::chrono::nanoseconds(PerfClock::now() - start);
    const auto speed = content.size() * NsPerS / duration.count() / Mebibyte;
    clobberMemory();

    std::cout
        << "{\"algo\": \"" STR(TEST_ALGO)
        << "\", \"input_file\": \"" << filename
        << "\", \"result\": " << res
        << ", \"input_size\": " << content.size()
        << ", \"time_ns\": " << duration.count()
        << ", \"mib_per_s\": " << speed
        << "}\n";

    return 0;
}
