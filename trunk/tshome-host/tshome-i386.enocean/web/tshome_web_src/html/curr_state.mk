include ../libs.mk
#目标信息
objs = $(base_obj) curr_state.o 
basic_control = $(obj_path)/curr_state.html

#开始编译
$(basic_control)	:	$(objs)
	$(TS_GCC) -Wall -o $(basic_control)  $(objs) $(imp_gateway) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
curr_state.o :
	$(TS_GCC) -Wall -c $(html_path)/curr_state.c $(imp_gateway)  -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(basic_control) curr_state.o 
