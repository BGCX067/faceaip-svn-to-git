include ../libs.mk
#目标信息
objs = $(base_obj) set_comm.o
set_comm = $(obj_path)/set_comm.html

#用户信息修改
$(set_comm) : $(objs) 
	$(TS_GCC) -Wall -o $(set_comm)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
set_comm.o :
	$(TS_GCC) -Wall -c $(src_path)/set_comm.c -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(set_ip) set_comm.o
