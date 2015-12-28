#!/bin/bash
# Author:       W
# Purpose:      run prg forever

__c=1
d=1
MONFILES="./mon.log"
USER_COMMAND="./tshome-p2p-server-linux-x86-32"

if [ -z "$1" ];then
	echo "use default cmd:"${USER_COMMAND}
else
	USER_COMMAND=$1
	echo "user cmd:"${USER_COMMAND}
fi

while [ $__c -le 5 ]
do
	echo "Welcome $d times ============= " >> ${MONFILES}
	date >> ${MONFILES}
	${USER_COMMAND}
	(( d++ ))
	sleep 1s
done
