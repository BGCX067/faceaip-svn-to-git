include ../libs.mk
#目标信息
objs = $(base_obj) login.o 
login = $(obj_path)/login.html

#开始编译
$(login)	:	$(objs)
	$(TS_GCC) -Wall -o $(login)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
login.o :
	$(TS_GCC) -Wall -c $(html_path)/login.c -I $(path_comm) #$(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(login) login.o 
