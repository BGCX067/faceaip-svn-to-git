include ../libs.mk
#目标信息
objs = $(base_obj) basic_control.o 
basic_control = $(obj_path)/basic_control.html

#开始编译
$(basic_control)	:	$(objs)
	$(TS_GCC) -Wall -o $(basic_control)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
basic_control.o :
	$(TS_GCC) -Wall -c $(html_path)/basic_control.c -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(basic_control) basic_control.o 
