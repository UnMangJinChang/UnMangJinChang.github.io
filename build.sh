#!/usr/bin/env bash
clang++ -g -o main-content-creator -std=c++20 main-content-creator.cpp
./main-content-creator
clang++ -g -o page-creator -std=c++20 page-creator.cpp
./page-creator
