#!/bin/bash

ROOTDIR=/opt/karuta-server
LOGDIR=/var/log/karuta-server
USER=karuta

cd $(dirname $0)

# Copy binaries
if [ -f "../bin/release/karuta-server" ] && [ -f "../bin/release/libkaruta.so" ]; then
	mkdir -p $ROOTDIR
	cp ../bin/release/karuta-server $ROOTDIR
	cp ../bin/release/libkaruta.so $ROOTDIR
else
	echo "karuta-server hasn't been built."
	return 1
fi

# Create log directory
mkdir -p $LOGDIR

# Generate service script
cat initd-header.sh > karuta-server
echo "" >> karuta-server
echo "ROOTDIR=$ROOTDIR" >> karuta-server
echo "LOGDIR=$LOGDIR" >> karuta-server
echo "USER=$USER" >> karuta-server
cat initd-body.sh >> karuta-server

# Install service script
mv karuta-server /etc/init.d/karuta-server
chmod 755 /etc/init.d/karuta-server
update-rc.d karuta-server defaults
