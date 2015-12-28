include ../libs.mk
#目标信息
objs = $(base_obj) login_validate.o 
login_validate = $(obj_path)/login_validate.html

#用户信息修改 
$(login_validate) : $(objs)
	$(TS_GCC) -Wall -o $(login_validate)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
login_validate.o :
	$(TS_GCC) -Wall -c $(src_path)/login_validate.c -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(objs) $(login_validate)
