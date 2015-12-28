src_path="../"
target_path="./"

html_src_path=${src_path}"html/"
html_target_path=${target_path}"html/"

make -f ${html_src_path}/login.mk
make -f ${html_src_path}/basic_control.mk
make -f ${html_src_path}/home_serial_port.mk
make -f ${html_src_path}/infrared_device_pathway.mk
make -f ${html_src_path}/pathway_info.mk
make -f ${html_src_path}/device_link_access.mk
make -f ${html_src_path}/log.mk
make -f ${html_src_path}/base_info.mk
make -f ${html_src_path}/guide_info.mk
make -f ${html_src_path}/curr_state.mk

