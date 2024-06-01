#!/bin/bash

MIMALLOC_URL="https://github.com/microsoft/mimalloc.git"
EXIT_FAILURE_NUM=1

if [[ -z "$1" ]]; then
    printf "ERROR - please supply mimalloc download directory as first argument!\n"
else
    mimalloc_build_subdir="out/secure"
    rm -rf "$1"
    git clone "$MIMALLOC_URL" "$1" &&
        current_root_dir="$(pwd)" &&
        cd "$1" &&
        git checkout master &&
        mkdir -p "$mimalloc_build_subdir" &&
        cd "$mimalloc_build_subdir" &&
        cmake -DMI_SECURE=ON "../.." &&
        make
    cd "$current_root_dir" || exit "$EXIT_FAILURE_NUM"
fi
