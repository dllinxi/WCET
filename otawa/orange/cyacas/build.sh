#!/bin/sh
cd "$(dirname 0)"
yacas-build/build.sh
./configure --with-yacas="${PWD}/yacas-inst/" $*
make
make install

