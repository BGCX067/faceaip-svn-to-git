include ../libs.mk
#目标信息
objs = $(base_obj) set_user_info.o
set_ip = $(obj_path)/set_user_info.html

#用户信息修改
$(set_ip) : $(objs) 
	$(TS_GCC) -Wall -o $(set_ip)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
set_user_info.o :
	$(TS_GCC) -Wall -c $(src_path)/set_user_info.c -I $(path_comm) 
#清除
clean:
	rm -rf $(set_ip) set_user_info.o
