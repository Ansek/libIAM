#!/bin/sh
mkdir build
git config --global --add safe.directory $PWD/build/_deps/unity-src
git config --global --add safe.directory $PWD/build/_deps/fff-src