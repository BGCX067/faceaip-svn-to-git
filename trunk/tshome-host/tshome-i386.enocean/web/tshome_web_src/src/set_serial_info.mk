include ../libs.mk
#目标信息
objs = $(base_obj) set_serial_info.o
set_serial_info = $(obj_path)/set_serial_info.html

#用户信息修改
$(set_serial_info) : $(objs) 
	$(TS_GCC) -Wall -o $(set_serial_info)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
set_serial_info.o :
	$(TS_GCC) -Wall -c $(src_path)/set_serial_info.c -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(set_ip) set_serial_info.o
