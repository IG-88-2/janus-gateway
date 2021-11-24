#! /bin/bash

sh autogen.sh

eval "./configure --prefix=/opt/janus --disable-data-channels --disable-rabbitmq --disable-mqtt --enable-boringssl" 

make --always-make && sudo make install

make configs