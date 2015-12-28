#!/bin/sh

#get url from pro
CURR_PATH=`pwd`
BAK_DIR=${CURR_PATH}/../../bak/tshome-host
HOST_MONITOR=ts_monitor


logger -t "bak.sh" ""
logger -t "bak.sh" "Start rollback..."
killall upd.sh
killall run_upd.sh
killall ${HOST_MONITOR}
sleep 5s

cd ../..
cp -rpf ${BAK_DIR}/tshome-host .
cd ${CURR_PATH}

../bin/${HOST_MONITOR} &
logger -t "bak.sh" "Rollback over"

#make flag
#echo "0" > ../flag_file
rm ../flag_file
