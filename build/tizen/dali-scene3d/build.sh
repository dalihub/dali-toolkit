#!/bin/bash

git clean -xdf .
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .
make install -j16
