#!/usr/bin/env bash

mkdir -p build/bin/

clang -O3 -c arena.c -o build/arena.o
clang -O3 -mavx2 -c arena_avx2.c -o build/arena_avx2.o
clang -O3 -msse2 -c arena_sse2.c -o build/arena_sse2.o
# clang -O3 -c arena_generic.c -o arena_generic.o

ar rcs build/bin/libarena.a build/arena.o build/arena_avx2.o build/arena_sse2.o
