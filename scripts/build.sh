#!/bin/bash

mkdir -p build && cd build \
&& cmake .. && cmake --build . --parallel 4 \
&& mkdir -p ../bin/ && mv ./console/*-console ../bin/
