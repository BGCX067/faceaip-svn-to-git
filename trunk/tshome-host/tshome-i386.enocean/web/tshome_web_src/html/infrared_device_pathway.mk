include ../libs.mk
#目标信息
objs = $(base_obj) infrared_device_pathway.o 
basic_control = $(obj_path)/infrared_device_pathway.html

#开始编译
$(basic_control)	:	$(objs)
	$(TS_GCC) -Wall -o $(basic_control)  $(objs) -I $(path_comm)  $(imp_gateway) $(ts_db_lib) $(lib_cgic) 
infrared_device_pathway.o :
	$(TS_GCC) -Wall -c $(html_path)/infrared_device_pathway.c -I $(path_comm) $(imp_gateway) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(basic_control) infrared_device_pathway.o 
