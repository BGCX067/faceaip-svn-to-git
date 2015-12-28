include ../libs.mk
#目标信息
objs = $(base_obj) imp_xml.o
obj_html = $(obj_path)/imp_xml.html

#用户信息修改
$(obj_html) : $(objs) 
	$(TS_GCC) -Wall -o $(obj_html)  $(objs) -I $(path_comm)  $(ts_db_lib) $(lib_cgic)
imp_xml.o :
	$(TS_GCC) -Wall -c $(src_path)/imp_xml.c -I $(path_comm) $(ts_db_lib) $(lib_cgic)
#清除
clean:
	rm -rf $(set_ip) imp_xml.o
