include ../libs.mk
#目标信息
objs = $(base_obj) set_channel.o
set_channel = $(obj_path)/set_channel.html

#用户信息修改
$(set_channel) : $(objs) 
	$(TS_GCC) -Wall -o $(set_channel)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic) 
set_channel.o :
	$(TS_GCC) -Wall -c $(src_path)/set_channel.c -I $(path_comm) $(ts_db_lib) $(lib_cgic) 
#清除
clean:
	rm -rf $(set_ip) set_channel.o
