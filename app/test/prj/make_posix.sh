#!/bin/bash

mkdir posix;
cp "cmake/CMakeLists.txt" "posix/CMakeLists.txt";

cd posix;

cmake -G "Unix Makefiles";

