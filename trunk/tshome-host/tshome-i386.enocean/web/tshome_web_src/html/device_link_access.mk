include ../libs.mk
#目标信息
objs = $(base_obj) device_link_access.o 
basic_control = $(obj_path)/device_link_access.html

#开始编译
$(basic_control)	:	$(objs)
	$(TS_GCC) -Wall -o $(basic_control)  $(objs) -I $(path_comm) $(ts_db_lib) $(lib_cgic)
device_link_access.o : 
	$(TS_GCC) -Wall -c $(html_path)/device_link_access.c -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(basic_control) device_link_access.o 