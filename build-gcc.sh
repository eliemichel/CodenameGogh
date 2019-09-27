#!/bin/bash

# Get git submodules
# This only has to run once, you can then comment it out if it takes too long
git submodule update --init --recursive

# Place all build related files in a specific directory.
# Whenever you'd like to clean the build and restart it from scratch, you can
# delete this directory without worrying about deleting important files.
mkdir build-gcc
cd build-gcc

# Call cmake to generate the Makefile. You can then build with 'make' and
# install with 'make install'
# If Qt is not found, you can specify its location using
# e.g. -DQt5_DIR=E:/Qt/5.9.2/msvc2017_64/lib/cmake/Qt5
cmake .. -DCMAKE_BUILD_TYPE=Debug -DQt5_DIR=E:/Qt/5.12.1/5.12.1/msvc2017_64/lib/cmake/Qt5

# Check that it run all right
if [ $? -eq 0 ]
then
	echo [92mSuccessful[0m
else
	echo [91mUnsuccessful[0m
fi
