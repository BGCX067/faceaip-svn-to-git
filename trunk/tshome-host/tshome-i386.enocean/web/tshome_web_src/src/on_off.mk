include ../libs.mk
#目标信息
objs = $(base_obj) on_off.o 
on_off = $(obj_path)/on_off.html

#用户信息修改
$(on_off) : $(objs)
	$(TS_GCC) -Wall -o $(on_off)  $(objs) $(imp_gateway)  -I $(path_comm) $(ts_db_lib) $(lib_cgic)
on_off.o :
	$(TS_GCC) -Wall -c $(src_path)/on_off.c $(imp_gateway) -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(on_off) $(on_off)
