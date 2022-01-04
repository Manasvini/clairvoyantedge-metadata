#!/bin/bash

# Print script commands and exit on errors.
set -xe

[ ! -d vcpkg ] && git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh -disableMetrics

echo "export VCPKG_ROOT=\"$HOME/vcpkg\"" >> .bashrc
echo "export PATH=\$PATH:$HOME/vcpkg/downloads/tools/cmake-3.20.1-linux/cmake-3.20.1-Linux-x86_64/bin/" >> .bashrc


./vcpkg/vcpkg install grpc \
 hiredis \
 redis-plus-plus[cxx17] \
 gtest \
 yaml-cpp \
 pugixml \
 curlpp \
 glog
