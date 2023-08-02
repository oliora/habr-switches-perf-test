#! /bin/bash

num_runs=${1-10}
algos=( $(for i in build/algo-*; do [ -f "$i" ] && echo "$i"; done) )

function generate_long_input() {
    >&2 echo "Generating long input file 'input/long.txt'..."
    for i in {1..100}; do
        cat "input/wp.txt" >> "input/long.txt"
    done
}

function run_benchmark() {
    for a in "${algos[@]}"; do
        >&2 echo "Run algo '$a' for '$1': ${num_runs} runs x ${2} passes..."
        for ((n=0; n<${num_runs}; n++)); do
            $a $1 $2
        done
    done
}

[[ -f "input/long.txt" ]] || generate_long_input
run_benchmark "input/wp.txt" 10
run_benchmark "input/long.txt" 1
