#!/bin/bash -e
# shellcheck disable=SC1111

declare -r desc="Compilation failure “$1”"
shift
declare -r out="$1"
shift
declare -ra cmd=("$@")

if "${cmd[@]}" 2> "$out"; then
    echo "FAILED: $desc"
    exit 1
else
    echo "PASSED: $desc"
fi
