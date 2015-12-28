include ../libs.mk
#目标信息
objs = $(base_obj) set_ip_port.o
set_ip = $(obj_path)/set_ip_port.html

#用户信息修改
$(set_ip) : $(objs) 
	$(TS_GCC) -Wall -o $(set_ip)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
set_ip_port.o :
	$(TS_GCC) -Wall -c $(src_path)/set_ip_port.c -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(set_ip) set_ip_port.o
