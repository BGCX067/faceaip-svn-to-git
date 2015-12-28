#!/bin/bash

PRO_NAME=tshome-host
HOST_MONITOR=ts_monitor
TS_LOG_DEBUG="logger -t "guide_upd""
PRO_PATH=/opt/tshome-host/
FLAG_FILE=${PRO_PATH}/conf/flag_file
TS_PING_IPS=(8.8.8.8 8.8.4.4 www.gov.cn www.baidu.com www.qq.com)
IPS_LEN=${#TS_PING_IPS[@]}
ret=0

${TS_LOG_DEBUG} "start guide update"

#check whether can ping server.
i=0
while [ $i -lt $IPS_LEN ] ; do
	ping ${TS_PING_IPS[$i]} -c 1
	ret=$?
	if [ $ret = 0 ] ; then
		break
	fi
	i=$(($i+1))
done
if [ $ret != 0 ] ; then
	${TS_LOG_DEBUG} "Can't connect to server"
	exit $ret
fi

ret=0
#read flag file,start count after flag is 1,end when flag back to 0
MAX_TRY=90
i=0
flag=0
is_start_upd=0

#If ts_monitor is running ,kill it
killall upd.sh
killall run_upd.sh
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


${TS_LOG_DEBUG} "Start monitor"
rm ${FLAG_FILE}
${PRO_PATH}bin/ts_monitor &
${TS_LOG_DEBUG} "Start count times"
ret=0
is_pro_started=0
is_pro_killed=0
is_flag_changed=0
is_upd_now=0
is_bak=0
#if pro started and killed but flag have not changed
while [ $i -lt $MAX_TRY ] ; do
	sleep 1s
	killall -0 ${PRO_NAME}
	curr_state=$?
	#when programe not stated
	if [ $is_pro_started = 0 ] ; then
		if [ $curr_state = 0 ] ; then
			is_pro_started=1
		fi
		continue
	#when programe started
	else
		if [ $curr_state != 0 ] ; then
			is_pro_killed=1	
		fi
	fi
	

	if [ -f ${FLAG_FILE}  ]; then
		flag=`awk -F : 'NR==1 {print substr($1,6,1)}' ${FLAG_FILE}`
	else
		${TS_LOG_DEBUG} "Flag file is not exits"
	fi

	${TS_LOG_DEBUG} "check flag time is "${i}", flag is "${flag}",flag is changed "${is_flag_changed} "pro is killed" ${is_pro_killed} ", now"
	if [ ${is_start_upd} = 0 ] ; then
		if [ ${flag} = "1" ] ; then
			ret=1
			is_start_upd=1
			is_flag_changed=1
			${TS_LOG_DEBUG} "Have update"
		fi
	else
		if [ ${flag} = "0" ] ; then
			${TS_LOG_DEBUG} "Guide update over"
			is_start_upd=0
			ret=0
			break
		fi
	fi
	
	if [ ${is_pro_killed} = 1 ] ; then
		if [ ${is_flag_changed} = 0 ] ; then
			${TS_LOG_DEBUG} "No update ,but killed"
			ret=1
			break
		fi
	fi

	#if is upd now , not set time
	killall -0 run_upd.sh
	is_upd_now=$?
	if [ $is_upd_now = 0 ] ; then 
		${TS_LOG_DEBUG} "run_upd.sh is running,do not set time"
		continue
	fi

	killall -0 upd.sh
	is_upd_now=$?
	if [ $is_upd_now = 0 ] ; then 
		${TS_LOG_DEBUG} "upd.sh is running,do not set time"
		continue
	fi

	#if bak now,upd failed
	killall -0 bak.sh
	is_bak=$?
	if [ $is_bak = 0 ] ; then 
		${TS_LOG_DEBUG} "bak.sh is running,do not set time"
		ret=1
		break
	fi

	i=$(($i+1))
done


exit $ret
