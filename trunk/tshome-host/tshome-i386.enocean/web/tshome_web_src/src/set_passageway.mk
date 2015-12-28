include ../libs.mk
#目标信息
objs = $(base_obj) set_passageway.o
set_passageway = $(obj_path)/set_passageway.html

#用户信息修改
$(set_passageway) : $(objs) 
	$(TS_GCC) -Wall -o $(set_passageway)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic) 
set_passageway.o :
	$(TS_GCC) -Wall -c $(src_path)/set_passageway.c -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(set_ip) set_passageway.o
