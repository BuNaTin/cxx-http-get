#!/bin/bash

mkdir -p build && cd build \
&& cmake .. -DSTATIC_ANALYS=1 && cmake --build . --parallel 4 \
&& mkdir -p ../bin/ && mv ./console/*-console ../bin/
