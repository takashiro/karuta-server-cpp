#!/bin/bash

ROOTDIR=$(pwd)

if [ -d extension ]; then
	for ext in $(ls extension); do
		echo "--- Making $ext ---"
		cd extension/$ext
		make $@
		cd $ROOTDIR
	done
fi
