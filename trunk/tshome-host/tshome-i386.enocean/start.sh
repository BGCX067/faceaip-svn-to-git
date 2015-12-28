CURR_PATH=/opt/tshome-host/
ret=0

if [ -f ${CURR_PATH}flag_file ] ; then
	ret=`awk '{print substr($1,6,1)}' ${CURR_PATH}flag_file`
fi

if [ ${ret} = 1 ] ; then
	logger -t "start.sh" "upd err ,back up"
	cd ${CURR_PATH}/update
	./bak.sh
fi

#start web monitor
cd /opt/tshome-host/web/goahead/LINUX
killall ts_web_monitor
./ts_web_monitor webs eth1&

#start host monitor
cd ${CURR_PATH}bin
if [ -f start_monitor.sh ] ; then
	./start_monitor.sh
fi
