#!/bin/bash
source common/build.inc

# This shell script is used to build the TSHome Project
# Created by jyren on 2012-04-10

CURRENT_PATH=`pwd`
COMMON_PATH=${CURRENT_PATH}/common
LOG_PATH=${CURRENT_PATH}/log
DEVICE_CTRL_PATH=${CURRENT_PATH}/dev_ctrl
ENC_DEC_PATH=${CURRENT_PATH}/encrypt_decrypt
HOST_DB_OP_PATH=${CURRENT_PATH}/host_db_op
P2P_PATH=${CURRENT_PATH}/p2p
WEB_PATH=${CURRENT_PATH}/web
MONITOR_PATH=${CURRENT_PATH}/monitor
UPD_PATH=${CURRENT_PATH}/update
OUTPUT_PATH=/opt/tshome-host
BAK_PATH=/opt/bak/tshome-host
UPDATE_DOWNLOAD=/opt/update/tshome-host/

# To make sure user input parameter is ARM or X86
if [ $# -ne 1 ];then
    echo "USAGE: ./build.sh [ARM|X86]"
    exit 0
fi

ARCH=$1

if [ $ARCH != "X86" -a $ARCH != "ARM" ];then
    echo "Only X86 or ARM are supported"
    exit 0
fi

rm -rf ${OUTPUT_PATH}

#make bak
rm -rf ${BAK_PATH}
echo "Create bak path"
mkdir -p ${BAK_PATH}

#make update dir
rm -rf ${UPDATE_DOWNLOAD}
echo "Create download path"
mkdir -p ${UPDATE_DOWNLOAD}

#make dirs
if [ ! -d ${OUTPUT_PATH} ];then
	mkdir -p ${OUTPUT_PATH}/bin
	mkdir -p ${OUTPUT_PATH}/conf
	mkdir -p ${OUTPUT_PATH}/db
	mkdir -p ${OUTPUT_PATH}/include
	mkdir -p ${OUTPUT_PATH}/lib
	mkdir -p ${OUTPUT_PATH}/web
	mkdir -p ${OUTPUT_PATH}/update
	cp -p ${HOST_DB_OP_PATH}/tshome.db ${OUTPUT_PATH}/db/
fi

# 0. copy common *.h
cd ${COMMON_PATH}
rm -rf ${OUTPUT_PATH}/include/common
mkdir ${OUTPUT_PATH}/include/common
cp -fp ./*.h ${OUTPUT_PATH}/include/common

rm -rf ${OUTPUT_PATH}/lib
mkdir ${OUTPUT_PATH}/lib

# Build 
cd ${LOG_PATH}
rm -rf ${OUTPUT_PATH}/include/log
mkdir ${OUTPUT_PATH}/include/log
cp -fp ./*.h ${OUTPUT_PATH}/include/log/
make clean -f Makefile-${ARCH};
make -f Makefile-${ARCH}
cp -fp ./*.a ${OUTPUT_PATH}/lib/

# 1. Build database operation lib
echo "Build database operation lib..."
cd ${HOST_DB_OP_PATH}
rm -rf ${OUTPUT_PATH}/include/db
mkdir ${OUTPUT_PATH}/include/db
cp -fp ./*.h ${OUTPUT_PATH}/include/db/
make -f Makefile-${ARCH} clean;
make -f Makefile-${ARCH}
#make clean -f Makefile-Web-${ARCH}.mk;
#make -f Makefile-Web-${ARCH}.mk
cp -fp ./*.a ${OUTPUT_PATH}/lib/


# 2. Build encryption and decrypting lib
echo "Build encryption and decrypting lib..."
cd ${ENC_DEC_PATH}
rm -rf ${OUTPUT_PATH}/include/encode
mkdir ${OUTPUT_PATH}/include/encode
cp -fp ./*.h ${OUTPUT_PATH}/include/encode/
make -f Makefile-${ARCH} clean
make -f Makefile-${ARCH}
cp -fp ./*.a ${OUTPUT_PATH}/lib/

# 3. Build device control lib
echo "Build device control lib..."
cd ${DEVICE_CTRL_PATH}
make -f Makefile-${ARCH} clean
make -f Makefile-${ARCH} 
cp -fp ./GatewayManager/*.so ${OUTPUT_PATH}/lib/
cp -fp ./Epson_Esc_Vp21/*.so ${OUTPUT_PATH}/lib/
cp -fp ./Auxdio_8150/*.so ${OUTPUT_PATH}/lib/
cp -fp ./Backaudio_A61L/*.so ${OUTPUT_PATH}/lib/
cp -fp ./Yodar_4028B/*.so ${OUTPUT_PATH}/lib/
cp -fp ./ISTV/*.so ${OUTPUT_PATH}/lib/
cp -fp ./AQM_1000/*.so ${OUTPUT_PATH}/lib/
rm -rf ${OUTPUT_PATH}/include/dev_ctrl
mkdir ${OUTPUT_PATH}/include/dev_ctrl
cp -fp ./GatewayManager/Conversion.h ./GatewayManager/KNXCondef.h ./GatewayManager/KNXWinVarType.h ${OUTPUT_PATH}/include/dev_ctrl/
mkdir ${OUTPUT_PATH}/include/dev_ctrl/enocean
cp -fp ./GatewayManager/enocean/enocean.h  ${OUTPUT_PATH}/include/dev_ctrl/enocean

# 4. Build Web
echo "Build Web..."
cd ${WEB_PATH}
cp -p start.sh /opt/tshome-host/
cp -p webs-${ARCH} goahead/LINUX/webs
chmod 755 goahead/LINUX/webs
cp -r goahead/ ${OUTPUT_PATH}/web/
cd tshome_web_src/build
./build.sh ${ARCH}

WEB_TARGET=${OUTPUT_PATH}/web/goahead/web
cp -rpf ./html/*.html ${WEB_TARGET}/cgi-bin/
cp -rpf ./html/*.sh ${WEB_TARGET}/cgi-bin/
cp -rpf ../js/* ${WEB_TARGET}/js/
cp -rpf ../css/* ${WEB_TARGET}/css/
cp -rpf ./images/* ${WEB_TARGET}/images/
chmod 755 ${WEB_TARGET}/cgi-bin/*.html
chmod 755 ${WEB_TARGET}/cgi-bin/*.sh

# 5. Build pjlib, pjlib-util, pjnath lib and communicate protocol
cd ${P2P_PATH}

cd pjnath/build
rm -rf output/* ../bin/* ../lib/*
cd ../..
cd pjlib/build
rm -rf output/* ../bin/* ../lib/*
cd ../..
cd pjlib-util/build
rm -rf output/* ../bin/* ../lib/*
cd ../..

./configure-${ARCH}
make -f Makefile-${ARCH} dep
make -f Makefile-${ARCH} clean
make -f Makefile-${ARCH}
cp -fp ./pjlib/lib/*.a ${OUTPUT_PATH}/lib/
cp -fp ./pjlib-util/lib/*.a ${OUTPUT_PATH}/lib/
cp -fp ./pjnath/lib/*.a ${OUTPUT_PATH}/lib/
rm -rfp ${OUTPUT_PATH}/include/pjnath
mkdir ${OUTPUT_PATH}/include/pjnath
cp -rpf ./pjnath/include ${OUTPUT_PATH}/include/pjnath/
rm -rpf ${OUTPUT_PATH}/include/pjlib
mkdir ${OUTPUT_PATH}/include/pjlib
cp -rpf ./pjlib/include ${OUTPUT_PATH}/include/pjlib/
rm -rf ${OUTPUT_PATH}/include/pjlib-util
mkdir ${OUTPUT_PATH}/include/pjlib-util
cp -rpf ./pjlib-util/include ${OUTPUT_PATH}/include/pjlib-util/
cp -pf ./pjnath/bin/tshome-host* ${OUTPUT_PATH}/bin/

# 6. Build the Monitor
cd ${MONITOR_PATH}
make -f Makefile-${ARCH} clean
make -f Makefile-${ARCH}
cp -pf ts_monitor ${OUTPUT_PATH}/bin
cp -pf ts_web_monitor ${OUTPUT_PATH}/web/goahead/LINUX

# 8.Build upd programe
cd ${UPD_PATH}
./build.sh ${ARCH}
cp -rpf ${UPD_PATH}/bin/* ${OUTPUT_PATH}/update

# 9. Tar the output
cd ${CURRENT_PATH}
cp -p install.sh /opt/
cp -p start.sh ${OUTPUT_PATH}/bin
#cp -p start.sh ${OUTPUT_PATH}/bin/bak_start.sh
cp -p set_net_infos.sh ${OUTPUT_PATH}
cd /opt
find -name "*.svn" -type d -exec rm -rf {} \;
mv tshome-host/bin/tshome-host-* tshome-host/bin/${PRO_NAME}
tar zcf tshome-host.tar.gz tshome-host install.sh

