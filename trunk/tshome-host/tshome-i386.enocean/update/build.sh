#build upd
ENV=$1
SRC=src
DST=bin

rm -rf ${DST}/*

# To make sure user input parameter is ARM or X86
if [ $# -ne 1 ];then
	echo "USAGE: ./build.sh [ARM|X86]"
	exit 0
fi

cd ${SRC}

if [ ${ENV} = "X86" ] ; then
	echo "Compline X86 update ..."
	make clean -f Makefile-X86
	make -f Makefile-X86
elif [ ${ENV} = "ARM" ] ; then
	echo "Compline ARM updatea ..."
	make clean -f Makefile-ARM
	make -f Makefile-ARM
else 
	echo "Check your envriotment!"
	exit 1
fi

cd ..

cp ${SRC}/*.sh ${DST}/
cp ${SRC}/*.ini ${DST}/
cp ${SRC}/conf ${DST}/
cp ${SRC}/download ${DST}/


exit 0

