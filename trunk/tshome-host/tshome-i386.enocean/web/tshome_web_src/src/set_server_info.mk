include ../libs.mk
#目标信息
objs = $(base_obj) set_server_info.o
set_ip = $(obj_path)/set_server_info.html

#用户信息修改
$(set_ip) : $(objs) 
	$(TS_GCC) -Wall -o $(set_ip)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
set_server_info.o :
	$(TS_GCC) -Wall -c $(src_path)/set_server_info.c -I $(path_comm) 
#清除
clean:
	rm -rf $(set_ip) set_server_info.o
