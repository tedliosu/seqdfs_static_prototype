#!/bin/bash

set -euo pipefail

MIMALLOC_URL="https://github.com/microsoft/mimalloc.git"
MIMALLOC_TAG="v3.3.2"
EXIT_FAILURE_NUM=1

if [[ "$UID" -eq 0 ]]; then
    printf "DO NOT run this script as root!\n"
    exit "$EXIT_FAILURE_NUM"
fi

_RESOLVED_PATH="$(realpath "$1")" || {
    printf "ERROR - Unable to resolve %s!\n" "$1"
    exit "$EXIT_FAILURE_NUM"
}
_RESOLVED_PATH_DIR="$(dirname "${_RESOLVED_PATH}")" || {
    printf "ERROR - Unable to resolve 'dirname' of %s!\n" "$1"
    exit "$EXIT_FAILURE_NUM"
}

printf "Checking if '%s' refers to a path inside a git repo..." "$1"
if ! git -C "${_RESOLVED_PATH_DIR}" rev-parse --is-inside-work-tree; then
    printf "ERROR - please make sure %s refers to a path inside the appropriate git repo!\n" "$1"
    exit "$EXIT_FAILURE_NUM"
else
    printf "\n"
    mimalloc_build_subdir="out/release"
    rm -rf "$1"
    git clone "$MIMALLOC_URL" "$1"
    git -C "$1" -c advice.detachedHead=false checkout "$MIMALLOC_TAG"
    git -C "$1" submodule update --init --recursive
    mkdir -p "$1/$mimalloc_build_subdir"
    cmake -S "$1" -B  "$1/$mimalloc_build_subdir"
    cmake --build "$1/$mimalloc_build_subdir"
fi
