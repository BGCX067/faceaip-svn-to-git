include ../libs.mk
#目标信息
objs = $(base_obj) home_serial_port.o 
basic_control = $(obj_path)/home_serial_port.html

#开始编译
$(basic_control)	:	$(objs)
	$(TS_GCC) -Wall -o $(basic_control)  $(objs) $(imp_gateway)  -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
home_serial_port.o :
	$(TS_GCC) -Wall -c $(html_path)/home_serial_port.c $(imp_gateway) -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(basic_control) home_serial_port.o 
