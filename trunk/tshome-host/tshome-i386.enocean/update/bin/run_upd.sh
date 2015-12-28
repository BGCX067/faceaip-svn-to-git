#!/bin/bash
#source ../../common/build.inc

PRO_NAME=tshome-host
CONF_HEAD=$1
AES_KEY=$2
SOFTWARE_URL=$3
NULL_STRING="null"
UPD_PATH=../../
UPD_CONF=conf_en
HOST_MONITOR=ts_monitor
OLD_DIR=../../../../tshome-host
FLAG_FILE=${OLD_DIR}/flag_file
ret=0
TS_LOG_DEBUG="logger -t "run_upd.sh""

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

#make flag
echo `sed '1 s/flag:0/flag:1/' ${FLAG_FILE} ` > ${FLAG_FILE}

#upd conf
if [ ${CONF_HEAD} != ${NULL_STRING} ] ;then
	ret=0
	${TS_LOG_DEBUG} " "
	${TS_LOG_DEBUG} "Start upd conf ...."
	./conf ${CONF_HEAD} ${AES_KEY}
	ret=$?
	if [ 0 != ${ret} ]; then
		${TS_LOG_DEBUG} "Upd conf failed"
		cd ${OLD_DIR}/update
		./bak.sh
		exit ${ret}
	fi
fi

#upd programe
if [ ${SOFTWARE_URL} != ${NULL_STRING} ] ; then
	${TS_LOG_DEBUG} " "
	${TS_LOG_DEBUG} "Start upd programe...."

	#run old delete
	${OLD_DIR}/update/delete.sh
	ret=$?
	if [ 0 != ${ret} ]; then
		${TS_LOG_DEBUG} "Delete old software failed"
		cd ${OLD_DIR}/update
		./bak.sh
		exit ${ret}
	fi
	
	${TS_LOG_DEBUG} "Start replace"
	cp -rf ${UPD_PATH}/tshome-host/* ${OLD_DIR}/
	ret=$?
	if [ 0 != ${ret} ]; then
		${TS_LOG_DEBUG} "Update software failed"
		cd ${OLD_DIR}/update
		./bak.sh
		exit ${ret}
	fi
fi

#run programe
${OLD_DIR}/bin/${HOST_MONITOR} &

#Whether the programe run 
i=0
pro_runed_flag=1
ret=0
while [ ${i} -lt 75 ] ; do
	i=$(($i+1))
	sleep 1s
	if [ ${pro_runed_flag} = 1 ] ;then
		killall -0 ${PRO_NAME}
		pro_runed_flag=$?
		${TS_LOG_DEBUG} "Programe started state : "${pro_runed_flag}
	fi
	
	#set is success.
	if [ ${pro_runed_flag} = 0 ] ;then
		killall -0 ${PRO_NAME}
		ret=$?
		${TS_LOG_DEBUG} "Programe check " ${i} "times, result : "${ret}
		if [ ${ret} != 0 ] ; then
			${TS_LOG_DEBUG} "Update failed"
			#Start bak
			cd ${OLD_DIR}/update
			./bak.sh
			exit ${ret}
		fi
	fi
done

#make flag
echo `sed '1 s/flag:1/flag:0/' ${FLAG_FILE} ` > ${FLAG_FILE}
echo `sed '1 s/is_success:1/is_success:0/' ${FLAG_FILE}` > ${FLAG_FILE}

sleep 5s
killall webs
#over
${TS_LOG_DEBUG} " "
${TS_LOG_DEBUG} "Update successfully"


exit ${ret}

