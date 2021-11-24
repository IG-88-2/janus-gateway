#!/bin/sh

# git clone --single-branch --branch master https://github.com/IG-88-2/janus-gateway 
# https://github.com/meetecho/janus-gateway
# cd janus-gateway

#sed -i 's/\r//g' autogen.sh
#dos2unix Makefile.am
#dos2unix Makefile
#dos2unix autogen.sh
#dos2unix configure.ac

find . -name \*.m4|xargs dos2unix
find . -name \*.ac|xargs dos2unix
find . -name \*.am|xargs dos2unix

sh autogen.sh

./configure --enable-post-processing --prefix=/opt/janus --disable-rest --disable-docs --disable-data-channels --disable-rabbitmq --disable-mqtt --disable-plugin-audiobridge --disable-plugin-echotest --disable-plugin-recordplay --disable-plugin-sip --disable-plugin-nosip --disable-plugin-streaming --disable-plugin-textroom --disable-plugin-videocall --disable-plugin-voicemail --enable-libsrtp2 --enable-boringssl

make
make install
make configs

#make --always-make --keep-going && make install
#make configs