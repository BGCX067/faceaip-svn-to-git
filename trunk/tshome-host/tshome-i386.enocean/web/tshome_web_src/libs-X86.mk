#基本编译信息
project_name=TS_FILE_DB
TS_GCC = gcc -D $(project_name) -D TS_X86
#libs
#libs_comm = ../../../output/lib/
libs_comm = /opt/tshome-host/lib
lib_sqlite = -lsqlite3 
lib_cgic = -lcgic
#include
path_comm = /opt/tshome-host/include
obj_path=./html
src_path=../src
#xml
path_xml = /usr/include/libxml2/
lib_xml = -L /usr/local/lib -lxml2 
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
