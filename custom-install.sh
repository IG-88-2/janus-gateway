#!/bin/sh

apt-get update
apt-get install -y git wget build-essential cmake libwebsockets-dev golang pkg-config libconfig-dev libmicrohttpd-dev libcurl4-openssl-dev libjansson-dev libnice-dev libssl-dev libglib2.0-dev libopus-dev libogg-dev gengetopt libtool automake
wget https://github.com/cisco/libsrtp/archive/v2.2.0.tar.gz
tar xfv v2.2.0.tar.gz
cd libsrtp-2.2.0
./configure --prefix=/usr --enable-openssl
make shared_library && make install
cd ..
rm v2.2.0.tar.gz
rm -rf libsrtp-2.2.0
git clone https://boringssl.googlesource.com/boringssl
cd boringssl
mkdir build
cd build
cmake ..
make
mkdir -p /opt/boringssl/lib/
cd ..
cp -R include /opt/boringssl/
cp build/ssl/libssl.a /opt/boringssl/lib/
cp build/crypto/libcrypto.a /opt/boringssl/lib/
cd ..
rm -rf boringssl
cd /
