#!/bin/sh

#get url from programe
cd /opt/tshome-host/update/

CURR_PATH=`pwd`
CONF_URL=$1
SOFTWARE_URL=$2
MD5_RES=$3
AES_KEY=$4
NULL_URL="null"
BAK_DIR=${CURR_PATH}/../../bak/tshome-host
BAK_FILE_NAME=host.tar.gz
UPD_PATH=${CURR_PATH}/../../update/tshome-host/
UPD_SOFT=host.tar.gz
UPD_CONF=conf_en
CONF_HEAD=${UPD_PATH}/${UPD_CONF}_head
RUN_UPD_DIR=run_upd
MD5_CURR=
MAX_RETRY=3
ret=0
i=0
FLAG_FILE=${CURR_PATH}/../flag_file
TS_LOG_DEBUG="logger -t "upd.sh""

killall -0 bak.sh
ret=$?
if [ $ret = 0 ] ; then
	exit 1
fi

killall -0 run_upd.sh
ret=$?
if [ $ret = 0 ] ; then
	exit 1
fi

ret=0
#Start delete old update info
rm -rf ${UPD_PATH}/*

echo "flag:0-is_success:1" > ${FLAG_FILE}

if [ ${NULL_URL} = ${CONF_URL}  -a ${NULL_URL} = ${SOFTWARE_URL} ] ; then
	${TS_LOG_DEBUG} "There is no thing want to update"
	exit 1
fi

#Download conf
if [ ${NULL_URL} != ${CONF_URL} ] ; then
	i=0
	while [ ${i} -lt ${MAX_RETRY} ] ; do
		i=$(($i+1))
		${TS_LOG_DEBUG} "i  = "$i
		#download file!
		${TS_LOG_DEBUG} " "
		${TS_LOG_DEBUG}  "Start download conf...."
		./download ${CONF_URL} ${UPD_PATH}/${UPD_CONF} ${CONF_HEAD}
		ret=$?
		if [ 0 != ${ret} ]; then
			${TS_LOG_DEBUG} "Download failed "${ret}
			continue
		else
			${TS_LOG_DEBUG} "Conf download success"
			break;
		fi
	done
	if [ ${ret} != 0 ] ; then
		${TS_LOG_DEBUG} "There is something wrong with download conf,check it"
		exit ${ret}
	fi
else
	${TS_LOG_DEBUG} "There is no configure to update"
fi

#Download software
if [ ${NULL_URL} != ${SOFTWARE_URL} ] ; then
	i=0
	#Download file 
	while [ ${i} -lt ${MAX_RETRY} ] ; do
		i=$(($i+1))
		#download file
		${TS_LOG_DEBUG} " "
		${TS_LOG_DEBUG}  "Start download file ...."
		./download ${SOFTWARE_URL} ${UPD_PATH}/${UPD_SOFT}
		ret=$?
		if [ 0 != ${ret} ]; then
			${TS_LOG_DEBUG} "Download failed "${ret}
			continue
		fi
	
		#validate programe
		if [ ${SOFTWARE_URL} != "null" ] ; then
			md5sum ${UPD_PATH}/${UPD_SOFT} > md5sum_res.txt
			MD5_CURR=`awk '{print substr($1,1,32)}' md5sum_res.txt`
			${TS_LOG_DEBUG} " "${MD5_CURR}
			if [ ${MD5_CURR} != ${MD5_RES} ] ; then
				ret=1
				${TS_LOG_DEBUG} "Validate error"
			else
				#validate success
				${TS_LOG_DEBUG} "Start tar the file...."
				cd ${UPD_PATH}/
				tar zxf host.tar.gz
				cd ${CURR_PATH}
				${TS_LOG_DEBUG} "Start del db"
				rm tshome-host/db/tshome.db
				break
			fi
		fi
	done
	if [ ${ret} != 0 ];then
		${TS_LOG_DEBUG} "There is something wrong with download file,check it"
		exit ${ret}
	fi
fi

#bak curr programe
${TS_LOG_DEBUG} "Start backup current programe ...."
rm -rf ${BAK_DIR}/*
cp -rpf ../../tshome-host ${BAK_DIR}/ 

#if there is no update software ,cp conf to update software
if [ ${SOFTWARE_URL} = ${NULL_URL} ] ; then
	mkdir -p ${UPD_PATH}tshome-host/update/
	cp -r * ${UPD_PATH}tshome-host/update/
fi

if [ ${CONF_URL} = ${NULL_URL} ] ; then
	CONF_HEAD=${NULL_URL}
fi

cd ${UPD_PATH}tshome-host/update/
./run_upd.sh ${CONF_HEAD} ${AES_KEY} ${SOFTWARE_URL}
ret=$?

exit $?

