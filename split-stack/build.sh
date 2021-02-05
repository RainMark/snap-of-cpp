#!/bin/bash

cc=clang++

FLAGS="-O0 -std=c++14 -fsplit-stack -fno-inline"
# FLAGS="-O0 -std=c++14"

# $cc -O0 -shared -fsplit-stack foo.cpp -o libfoo.so
$cc -O0 -shared foo.cpp -o libfoo.so

$cc $FLAGS main.cpp -L. -lfoo
