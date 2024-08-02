#! /usr/bin/env bash

find . -name "*.c" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format --style=file -i
