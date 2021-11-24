#!/bin/sh

rm -rf custom-configure
git clone --single-branch --branch master https://github.com/IG-88-2/configure custom-configure
cd custom-configure
gcc -w -g3 configure.c -o configure -lconfig

cp -a /opt/janus/etc/janus/. /debug-config
./configure --config_base /debug-config --nat_1_1_mapping 127.0.0.1 --debug_level 5 --rtp_port_range 20000-20100 --stun_server stun.voip.eutelia.it --stun_port 3478 --string_ids 1 --admin_key test --server_name instance_0 --ws_port 8188 --admin_ws_port 7188 --log_prefix instance_0:
