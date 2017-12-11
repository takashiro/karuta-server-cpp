#!/bin/bash

make clean
./sh/make-ext.sh clean

make
./sh/make-ext.sh

cd bin/release
cp -f karuta-server /opt/karuta-server
cp -f *.so /opt/karuta-server
cd ../../
