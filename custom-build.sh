#! /bin/bash

dos2unix Makefile.am

sh autogen.sh

eval "./configure --enable-post-processing --prefix=/opt/janus --disable-rest --disable-docs --disable-data-channels --disable-rabbitmq --disable-mqtt --disable-plugin-audiobridge --disable-plugin-echotest --disable-plugin-recordplay --disable-plugin-sip --disable-plugin-nosip --disable-plugin-streaming --disable-plugin-textroom --disable-plugin-videocall --disable-plugin-voicemail --enable-libsrtp2 --enable-boringssl"
#sudo apt-get install ffmpeg libavcodec-dev libavutil-dev libavformat-dev libswscale-dev
#make
#make install
#make configs

make install

make configs