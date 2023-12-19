#!/usr/bin/env bash
cur_hash=`echo -n main-content-creator.cpp | sha256sum | cut -d' ' -f1`
if [ ! -f ./main-content-creator-hash ] || [ `cat main-content-creator-hash` != ${cur_hash} ]; then
    clang++ -g -o main-content-creator -std=c++20 main-content-creator.cpp
fi
echo ${cur_hash} > main-content-creator-hash
./main-content-creator
cur_hash=`echo -n page-creator.cpp | sha256sum | cut -d' ' -f1`
if [ ! -f ./page-creator-hash ] || [ `cat page-creator-hash` != ${cur_hash} ]; then
    clang++ -g -o page-creator -std=c++20 page-creator.cpp
fi
echo ${cur_hash} > page-creator-hash
./page-creator
