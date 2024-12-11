#!/bin/bash
set -eux

day=$1

/usr/bin/clang++ -O3 -fcolor-diagnostics -fansi-escape-codes -g -pedantic-errors -Wall -Weffc++ \
-Wextra -Wconversion -Wsign-conversion -Werror -std=c++20 \
$day/**.cpp \
-o $day/main \

xctrace record --output . --template "Time Profiler" --time-limit 20s --launch \
-- $day/main