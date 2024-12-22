#!/bin/bash
set -eux

day=$1

clang-format -i -style=Google $day/**.cpp #$day/**.h 

/usr/bin/clang++ -O3 -fcolor-diagnostics -fansi-escape-codes -g -pedantic-errors -Wall -Weffc++ \
-Wextra -Wconversion -Wsign-conversion -Werror -std=c++20 \
$day/**.cpp \
-o $day/main \

time $day/main
