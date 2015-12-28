#!/bin/sh
OUTPUT_PATH=/opt/tshome-host
BAK_PATH=/opt/bak/tshome-host
UPDATE=/opt/update/tshome-host/

if [ ! -d ${OUTPUT_PATH} ]; then
    mkdir -p ${OUTPUT_PATH}
fi

#make bak
rm -rf ${BAK_PATH}
echo "Create bak path"
mkdir -p ${BAK_PATH}

#make update dir
rm -rf ${UPDATE}
echo "Create download path"
mkdir -p ${UPDATE}

killall ts_web_monitor

rm -rf ${OUTPUT_PATH}
echo "cp -rf tshome-host /opt"
cp -rf tshome-host /opt

echo "/opt/tshome-host/start.sh"
/opt/tshome-host/bin/start.sh
