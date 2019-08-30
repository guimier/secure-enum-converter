#!/bin/bash -e

if ! git diff --quiet tests/compile_fail/*.out; then
    tput setaf 1
    echo "WARNING: The following out files changed:"
    git diff --name-only tests/compile_fail/*.out | sed 's/^/\t/'
    echo
    echo "Please review the changes before considering the tests as passing."
    tput op
fi
