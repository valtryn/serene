#!/bin/sh

set -xe

CC="cc"
CFLAGS="-Wall -Wextra -Werror -Werror=maybe-uninitialized -ggdb -O0 -std=c11 -Wpedantic -Wvla -Wcast-align=strict -Wshadow -Wconversion -Wunreachable-code -Wformat=2 -Wno-incompatible-pointer-types"
OPTIMIZATION="-march=native -flto"
CDISABLEDFLAGS="-Wno-error=unused-variable -Wno-conversion"
PKGS="-lz -lpng"
# CFILES="allocator.c ds.c str.c"
CFILES="main.c allocator.c str.c"
PROGRAM="-o Serene"

$CC $CFLAGS $CDISABLEDFLAGS $PKGS $CFILES $PROGRAM
# ./build.sh && valgrind -s --leak-check=full --show-reachable=yes --track-origins=yes --leak-check=yes --tool=memcheck --read-var-info=yes ./Serene

time ./Serene
