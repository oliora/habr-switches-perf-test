#! /bin/bash

num_runs=${1-10}
input_dir=inputs
small_input_file=wp.txt
long_input_file=long.txt

if [ ! -f "${input_dir}/${long_input_file}" ]; then
    for ((n=0; n<100; n++)); do
        cat "${input_dir}/${small_input_file}" >> "${input_dir}/${long_input_file}"
    done
fi

algos=( $(for i in build/algo*; do [ -f "$i" ] && echo "$i"; done) )
inputs=( ${small_input_file} ${long_input_file} )

for a in "${algos[@]}"; do
    for i in "${inputs[@]}"; do
        for ((n=0; n<num_runs; n++)); do
            "$a" "${input_dir}/$i"
        done
    done
done