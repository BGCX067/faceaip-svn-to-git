include ../libs.mk

#目标信息
objs = $(base_obj) tshome_web.o ts_db_conn_infos.o ts_db_tools.o ts_db_web_infos.o sqlite3_ctrl.o ts_db_protocol_infos.o
tshome_web = tshome_web

#用户信息修改
$(tshome_web) : $(objs)
	$(TS_GCC) -Wall -o $(tshome_web)  $(objs) -I $(path_comm)  $(lib_sqlite) $(imp_xml) -L/opt/tshome-host/lib -lts_log_tools
tshome_web.o : $(src_path)/db/sqlite3_ctrl.h $(src_path)/db/ts_db_web_infos.h $(src_path)/db/ts_db_tools.h $(src_path)/db/ts_db_conn_infos.h
	$(TS_GCC) -Wall -c $(src_path)/tshome_web.c -I $(path_comm) $(lib_sqlite) 
#tools
ts_db_tools.o : $(src_path)/db/ts_db_tools.h
	$(TS_GCC) -Wall -c $(src_path)/db/ts_db_tools.c -I $(path_comm) $(imp_xml)
#数据库模块
ts_db_protocol_infos.o : $(src_path)/db/sqlite3_ctrl.h 
	$(TS_GCC) -Wall -c $(src_path)/db/ts_db_protocol_infos.c -I $(path_comm) $(lib_sqlite)
ts_db_web_infos.o : $(src_path)/db/sqlite3_ctrl.h 
	$(TS_GCC) -Wall -c $(src_path)/db/ts_db_web_infos.c -I $(path_comm) $(lib_sqlite)
sqlite3_ctrl.o : $(src_path)/db/sqlite3_ctrl.h
	$(TS_GCC) -Wall -c $(src_path)/db/sqlite3_ctrl.c -I $(path_comm) $(lib_sqlite)
ts_db_conn_infos.o : $(src_path)/db/sqlite3_ctrl.h 
	$(TS_GCC) -Wall -c $(src_path)/db/ts_db_conn_infos.c -I $(path_comm) $(lib_sqlite)
#清除
clean:
	rm -rf $(objs) $(tshome_web)
