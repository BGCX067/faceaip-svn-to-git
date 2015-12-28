ARM_PATH=/usr/local/arm/4.3.2
ARM_LIB=$(ARM_PATH)/lib
ARM_BIN=$(ARM_PATH)/bin
ARM_INC=$(ARM_PATH)/include
ARM_XML_INC=$(ARM_PATH)/include/libxml2
path_comm=-I /opt/tshome-host/include/
lib_sqlite=-L$(ARM_LIB) -lsqlite3
objs=sqlite3_ctrl.o ts_db_web_infos.o ts_db_tools.o ts_db_protocol_infos.o ts_db_conn_infos.o

lib_xml = -L$(ARM_LIB) -lxml2 
imp_xml = $(lib_xml) -I$(ARM_XML_INC)

obj=libts_db_tools_web.a

#WEB_GCC=$(ARM_BIN)/arm-linux-gcc -DTSHOME_WEB
WEB_GCC=$(ARM_BIN)/arm-linux-gcc 
AR=$(ARM_BIN)/arm-linux-ar

$(obj) : $(objs)
	$(AR) rs $(obj) $(objs)
sqlite3_ctrl.o :
	$(WEB_GCC) -Wall -c sqlite3_ctrl.c $(path_comm) -I$(ARM_INC)
ts_db_tools.o :
	$(WEB_GCC) -Wall -c ts_db_tools.c $(path_comm) -I$(ARM_INC) $(imp_xml)
ts_db_protocol_infos.o :
	$(WEB_GCC) -Wall -c ts_db_protocol_infos.c $(path_comm) -I$(ARM_INC)
ts_db_web_infos.o :  
	$(WEB_GCC) -Wall -c ts_db_web_infos.c $(path_comm) -I$(ARM_INC)
ts_db_conn_infos.o : 
	$(WEB_GCC) -Wall -c ts_db_conn_infos.c $(path_comm) -I$(ARM_INC)
clean:
	rm -f $(objs) $(obj)
