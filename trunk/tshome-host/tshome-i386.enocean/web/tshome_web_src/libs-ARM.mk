#基本编译信息
ARM_PATH=/usr/local/arm/4.3.2
ARM_BIN=${ARM_PATH}/bin
ARM_LIB=${ARM_PATH}/lib
ARM_INC=${ARM_PATH}/include
ARM_XML_INC=${ARM_PATH}/include/libxml2
project_name=TSHOME_WEB
TS_GCC = ${ARM_BIN}/arm-linux-gcc -D $(project_name) -D TS_ARM

#libs
libs_comm = /opt/tshome-host/lib
lib_sqlite = -L${ARM_LIB} -lsqlite3 
lib_cgic = -L${ARM_LIB} -lcgic

#include
path_comm = /opt/tshome-host/include -I${ARM_INC}
obj_path=./html
src_path=../src

#xml
path_xml = ${ARM_XML_INC}
lib_xml = -L${ARM_LIB} -lxml2 
imp_xml = $(lib_xml) -I $(path_xml)

#本html信息
html_path=../html

#so libs
gateway_lib = -ldl -L $(libs_comm) -lGatewayManager 
imp_gateway = #$(gateway_lib)
#log libs
ts_log_lib=-L $(libs_comm) -lts_log_tools

#db libs
ts_db_lib=-L $(libs_comm) -lts_db_tools $(lib_sqlite) $(imp_xml) ${ts_log_lib}
