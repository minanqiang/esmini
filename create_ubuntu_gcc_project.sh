#!/bin/bash

mkdir build
cd build
cmake ../ -DUSE_OSG=true -DUSE_OSI=true -DCMAKE_BUILD_TYPE=Release
cd ..
