#!/bin/sh
cd build &&\
cmake ../../.. -D IAM_INCLUDE_PLUGINS=ON -D IAM_BUILD_EXAMPLES=ON -D IAM_BUILD_TESTS=ON -D IAM_PLUGINS_DIR="/usr/local/lib/iam_plugins" &&\
echo -- Build -- &&\
cmake --build . &&\
echo -- Test -- &&\
ctest -C Debug -VV
cd ..