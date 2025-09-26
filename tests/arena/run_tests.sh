#!/usr/bin/env bash

mkdir -p build/bin/

clang src/main.c lib/libarena.a -o build/bin/main

./build/bin/main
