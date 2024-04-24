#!/bin/sh
cd build &&\
echo -- Install -- &&\
sudo cmake --install . &&\
echo -- Check -- &&\
./examples/plugin_viewer/plugin_viewer
cd ..