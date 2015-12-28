#import .so :update /etc/ld.so.conf;ldconfig

ARCH=$1
cp -f ../libs-${ARCH}.mk ../libs.mk
src_path="../"
target_path="./"

html_src_path=${src_path}"html/"
html_target_path=${target_path}"html/"

code_src_path=${src_path}"src"

js_src_path=${src_path}"js/"
js_target_path=${target_path}"js/"

css_src_path=${src_path}"css/"
css_target_path=${target_path}"css/"

script_src_path=${src_path}"scripts/"
script_target_path=${html_target_path}

#clear
rm *.o ${html_target_path}*.html
#logic html
make -f ${code_src_path}/login.mk
make -f ${code_src_path}/set_ip_port.mk
make -f ${code_src_path}/set_user_info.mk
make -f ${code_src_path}/set_serial_info.mk
make -f ${code_src_path}/set_channel.mk
make -f ${code_src_path}/on_off.mk
make -f ${code_src_path}/set_passageway.mk
make -f ${code_src_path}/set_comm.mk
make -f ${code_src_path}/imp_xml.mk
make -f ${code_src_path}/control_host.mk
make -f ${code_src_path}/set_server_info.mk
make -f ${code_src_path}/set_infr_driver.mk

#html to show
/bin/sh ${html_src_path}gcc_script.sh

#js copy
#cp -r ${js_src_path}* ${js_target_path}
#cp -r ${css_src_path}* ${css_target_path}

#script
cp -p ${script_src_path}* ${script_target_path}



