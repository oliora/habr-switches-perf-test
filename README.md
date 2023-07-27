# Prerequisites

- Install g++
- Install meson
- Install ninja (optionally)

# How to build

To build release version (fully optimized without debug symbols):

    meson setup build [-Db_sanitize=address,undefined]
    meson compile -C build


To build debug version (non-optimized with debug symbols):

    meson setup build-debug --buildtype=debug [-Db_sanitize=address,undefined]
    meson compile -C build-debug

# How to run tests

To run all the tests:

    ./runbenchmark > result.log

Or providing a number of runs for each test (10 is default number):

    ./runbenchmark 1 > result.log

For more precise results run tests on an isolated CPU:

    taskset -c <cpu_to_pin> ./runbenchmark <nun_runs> > result.log

# How to run unit tests and micro benchmarks

Bild release version first then run:

    meson test -C build -v

To run particular algo on a particular file run:

    build/algoXXX <input_file>
