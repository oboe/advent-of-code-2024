#!/bin/bash
set -eux

day=$1

/usr/bin/clang++ -fcolor-diagnostics -fansi-escape-codes -g -pedantic-errors -Wall -Weffc++ \
-Wextra -Wconversion -Wsign-conversion -Werror -std=c++20 \
/Users/shintaroonuma/workplace/advent-of-code/$day/**.cpp \
-o /Users/shintaroonuma/workplace/advent-of-code/$day/main \

/Users/shintaroonuma/workplace/advent-of-code/$day/main
