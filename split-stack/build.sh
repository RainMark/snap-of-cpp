#!/bin/bash

cc=clang++
FLAGS="-O0 -std=c++14 -fno-inline"

# $cc -shared -fsplit-stack $FLAGS foo.cpp -o libfoo.so
$cc -shared $FLAGS foo.cpp -o libfoo.so
$cc -fsplit-stack $FLAGS main.cpp -L. -lfoo
