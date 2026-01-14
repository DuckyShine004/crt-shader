#!/bin/bash

mkdir -p build

cmake -S . -B build -G Ninja

cmake --build build

rm -rf .cache/logs/*