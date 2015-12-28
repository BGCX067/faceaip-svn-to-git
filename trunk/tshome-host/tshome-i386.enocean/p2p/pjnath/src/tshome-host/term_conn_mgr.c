/*
 * term_conn_mgr.c
 *
 *  Created on: Feb 8, 2012
 *      Author: SongShanping
 */

#include "term_conn_mgr.h"
#include "common/thread.h"
#include "string.h"
#include "common/remote_def.h"

THREAD_MUTEX mux_lock_term_list = PTHREAD_MUTEX_INITIALIZER;

LIST_HEAD(terminal_list);


terminal_identify_info_t *get_terminal(int32_t session_id)
{
    LOCK_MUTEX(&mux_lock_term_list);
    terminal_info_t *rlist = NULL;
    list_for_each_entry(rlist,&terminal_list,list_node){
        terminal_identify_info_t *temp = &(rlist->terminal);
        if (temp->session_id == session_id){
            UNLOCK_MUTEX(&mux_lock_term_list);
            return temp;
        }
    }
    UNLOCK_MUTEX(&mux_lock_term_list);
    return NULL;
}

terminal_identify_info_t *get_terminal_by_name_pwd(char *username, char *pwd)
{
	LOCK_MUTEX(&mux_lock_term_list);
    terminal_info_t *rlist = NULL;
    list_for_each_entry(rlist,&terminal_list,list_node){
        terminal_identify_info_t *temp = &(rlist->terminal);
        if (strcmp(temp->user_name,username)==0 &&
			strcmp(temp->passwd,pwd)==0){
            UNLOCK_MUTEX(&mux_lock_term_list);
            return temp;
        }
    }
    UNLOCK_MUTEX(&mux_lock_term_list);
    return NULL;
}

int is_terminal_login(int32_t session_id)
{
    terminal_identify_info_t *term = get_terminal(session_id);
    if (NULL == term){
        return FALSE;
    }else{
        return TRUE;
    }
}

void add_terminal(terminal_identify_info_t *term_info)
{
    terminal_info_t *rlist = (terminal_info_t *)malloc(sizeof(terminal_info_t));
    if (NULL == rlist) {
        return;
    }

    terminal_identify_info_t *temp = &(rlist->terminal);
    temp->addr_len = term_info->addr_len;
    memcpy(temp->aes_key, term_info->aes_key, sizeof(term_info->aes_key));
    temp->comp_id = term_info->comp_id;
    temp->p_k = term_info->p_k;
    memcpy(temp->ip_addr, term_info->ip_addr, sizeof(term_info->ip_addr));
    temp->len_name = term_info->len_name;
    temp->len_pwd = term_info->len_pwd;
    memcpy(temp->passwd, term_info->passwd, sizeof(term_info->passwd));
    temp->session_id = term_info->session_id;
    memcpy(temp->user_name, term_info->user_name, sizeof(term_info->user_name));

    LOCK_MUTEX(&mux_lock_term_list);

    list_add_tail(&(rlist->list_node), &terminal_list);

    UNLOCK_MUTEX(&mux_lock_term_list);
}

int remove_terminal(int32_t session_id)
{
    LOCK_MUTEX(&mux_lock_term_list);

    terminal_info_t *rlist = NULL;
    terminal_info_t *alist = NULL;
    list_for_each_entry_safe(rlist, alist, &terminal_list, list_node){
        terminal_info_t *temp_list = rlist;
        terminal_identify_info_t *temp = &(temp_list->terminal);
        if (temp->session_id == session_id){
            list_del(&rlist->list_node);
            free(temp_list);
            temp_list = NULL;
            rlist = NULL;

            UNLOCK_MUTEX(&mux_lock_term_list);
            return SUCCESS;
        }
    }

    UNLOCK_MUTEX(&mux_lock_term_list);
    return FAILED;
}

void destroy_terminal_list()
{
    LOCK_MUTEX(&mux_lock_term_list);
    terminal_info_t *rlist = NULL;
    terminal_info_t *alist = NULL;
    list_for_each_entry_safe(rlist, alist, &terminal_list, list_node){
        terminal_info_t *temp = rlist;
        list_del(&rlist->list_node);
        free(temp);
        temp = NULL;
        rlist = NULL;
    }
    UNLOCK_MUTEX(&mux_lock_term_list);
}
