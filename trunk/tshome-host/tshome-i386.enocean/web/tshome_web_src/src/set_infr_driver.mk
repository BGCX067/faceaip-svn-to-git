include ../libs.mk
#目标信息
objs = $(base_obj) set_infr_driver.o
set_infr_driver = $(obj_path)/set_infr_driver.html

#用户信息修改
$(set_infr_driver) : $(objs) 
	$(TS_GCC) -Wall -o $(set_infr_driver)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic) 
set_infr_driver.o :
	$(TS_GCC) -Wall -c $(src_path)/set_infr_driver.c -I $(path_comm) $(ts_db_lib) $(lib_cgic) 
#清除
clean:
	rm -rf $(set_ip) set_infr_driver.o
