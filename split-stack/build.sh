#!/bin/bash

FLAGS="-O0 -std=c++14 -fsplit-stack -fno-inline"
# FLAGS="-O0 -std=c++14"

g++ $FLAGS main.cpp
