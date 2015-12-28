path_comm=-I /opt/tshome-host/include/
lib_sqlite=-lsqlite3
objs=sqlite3_ctrl.o ts_db_web_infos.o ts_db_tools.o ts_db_protocol_infos.o ts_db_conn_infos.o

path_xml = /usr/include/libxml2/
lib_xml = -L /usr/local/lib -lxml2 
imp_xml = $(lib_xml) -I $(path_xml)

obj=libts_db_tools_web.a

#tar -zxvf libxml2-2.7.2.tar.gz 
#cd libxml2-2.7.2
#./configure --prefix=/usr/include
#make clean
#make
#make install
#cd ..

#WEB_GCC=gcc -D TSHOME_WEB
WEB_GCC=gcc 

$(obj) : $(objs)
	ar cr $(obj) $(objs)
sqlite3_ctrl.o :
	$(WEB_GCC) -Wall -c sqlite3_ctrl.c $(path_comm) $(lib_sqlite) 
ts_db_tools.o :
	$(WEB_GCC) -Wall -c ts_db_tools.c $(path_comm) $(lib_sqlite) $(imp_xml)
ts_db_protocol_infos.o :
	$(WEB_GCC) -Wall -c ts_db_protocol_infos.c $(path_comm) $(lib_sqlite)
ts_db_web_infos.o :  
	$(WEB_GCC) -Wall -c ts_db_web_infos.c $(path_comm) $(lib_sqlite)
ts_db_conn_infos.o : 
	gcc -Wall -c ts_db_conn_infos.c $(path_comm) $(lib_sqlite)
clean:
	rm -f $(objs) $(obj)
