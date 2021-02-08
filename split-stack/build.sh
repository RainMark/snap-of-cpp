#!/bin/bash

cc=clang++
# FLAGS="-O0 -fuse-ld=lld -std=c++14 -fno-inline"
# FLAGS="-O0 -fuse-ld=gold -Wl,--split-stack-adjust-size=314572800 -std=c++14 -fno-inline"
FLAGS="-O0 -std=c++14 -fno-inline" # use ld

# $cc -shared -fsplit-stack $FLAGS foo.cpp -o libfoo.so
$cc -shared $FLAGS foo.cpp -o libfoo.so
$cc -fsplit-stack $FLAGS main.cpp -L. -lfoo
