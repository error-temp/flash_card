#!/bin/bash

flags="-std=c99"
out_dir="bin"
out="flash_card"

mkdir -p $out_dir
cc $flags -o $out_dir/$out src/*.c || exit 1
./$out_dir/$out $@
