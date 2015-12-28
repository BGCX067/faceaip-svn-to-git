/*
 * term_conn_mgr.h
 *
 *  Created on: Feb 8, 2012
 *      Author: SongShanping
 */

#ifndef TERM_CONN_MGR_H_
#define TERM_CONN_MGR_H_
#include <stdio.h>
#include <stdlib.h>
#include "common/list.h"
#include "common/common_define.h"

//struct of terminal info
typedef struct _terminal_info
{
    struct list_head list_node;  //list node
    terminal_identify_info_t terminal; //the struct of terminal identify info
}terminal_info_t;

// get terminal info by terminal session id.
terminal_identify_info_t *get_terminal(int32_t session_id);

// get terminal info by terminal username and pwd.
terminal_identify_info_t *get_terminal_by_name_pwd(char *username, char *pwd);

//check the terminal is whether had loged in by terminal sessiion id.
int is_terminal_login(int32_t session_id);

//add a terminal to terminal list.
void add_terminal(terminal_identify_info_t *term_info);

//remove a terminal from terminal list.
int remove_terminal(int32_t session_id);

//release terminal list.
void destroy_terminal_list();

#endif /* TERM_CONN_MGR_H_ */
