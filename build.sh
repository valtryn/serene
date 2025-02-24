#!/bin/sh

CC=cc
CFLAGS="-Wall -Wextra -Werror -std=c11 -Wpedantic -Wvla -Wcast-align=strict -Wshadow -Wconversion -Wunreachable-code -Wformat=2 -Wstrict-prototypes"
OPTIMIZATION="-O0"
DEBUG="-g"
CFILES="main.c allocator.c"
PROGRAM="-o serene"

$CC $CFLAGS $OPTIMIZATION $DEBUG $CFILES $PROGRAM

time ./serene
