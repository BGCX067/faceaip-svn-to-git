include ../libs.mk
#目标信息
objs = $(base_obj) control_host.o
control_host = $(obj_path)/control_host.html

#用户信息修改
$(control_host) : $(objs) 
	$(TS_GCC) -Wall -o $(control_host)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
control_host.o :
	$(TS_GCC) -Wall -c $(src_path)/control_host.c -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(control_host) control_host.o
