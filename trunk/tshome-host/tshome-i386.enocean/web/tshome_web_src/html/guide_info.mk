include ../libs.mk
#目标信息
objs = $(base_obj) guide_info.o 
basic_control = $(obj_path)/guide_info.html

#开始编译
$(basic_control)	:	$(objs)
	$(TS_GCC) -Wall -o $(basic_control)  $(objs) $(imp_gateway) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
guide_info.o :
	$(TS_GCC) -Wall -c $(html_path)/guide_info.c $(imp_gateway)  -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(basic_control) guide_info.o 
