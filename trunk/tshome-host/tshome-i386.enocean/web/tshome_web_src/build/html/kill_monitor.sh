#!/bin/sh
#!/bin/bash

PRO_NAME=tshome-host
HOST_MONITOR=ts_monitor
TS_LOG_DEBUG="logger -t "kill_monitor""
PRO_PATH=/opt/tshome-host/
ret=0

killall upd.sh
killall run_upd.sh

#If ts_monitor is running ,kill it
killall -0 ${HOST_MONITOR}
ret=$?
if [ ${ret} = 0 ] ; then
    ${TS_LOG_DEBUG} " " 
    ${TS_LOG_DEBUG} "kill monitor"
    killall ${HOST_MONITOR}
fi

pro_is_run=0
i=0;
while [ ${i} -lt 15 ] ; do
	i=$(($i+1))
	killall -0 ${PRO_NAME}
	pro_is_run=$?
	if [ ${pro_is_run} != 0 ] ; then
		break
	else 
		${TS_LOG_DEBUG} "Programe not die Quick Kill yourself"
	fi  
	sleep 1s
done
killall -9 ${PRO_NAME}

${TS_LOG_DEBUG} "Programe is die"

exit $ret
