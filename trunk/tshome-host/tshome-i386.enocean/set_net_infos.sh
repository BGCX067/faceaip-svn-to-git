NET_NAME=$1
IS_DHCP=$2
IP=$3
GW=$4
NM=$5

ret=0
#set dhcp
if [ $IS_DHCP = 1 ] ; then
	udhcpc -i $NET_NAME -t 5 -T 2 -S -n
	ret=$?
	if [ $ret = 0 ] ; then
		exit 0
	fi
fi

#SET IP
ifconfig  $NET_NAME $IP

#SET GW
route del default $NET_NAME 
route add default gw $GW $NET_NAME

#SET net mask
ifconfig $NET_NAME netmask $NM 

exit 0
