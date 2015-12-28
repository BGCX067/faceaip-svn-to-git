/*
 * update_mgr.h
 *
 *  Created on: Feb 18, 2012
 *      Author: SongShanping
 */

#ifndef UPDATE_MGR_H_
#define UPDATE_MGR_H_
#include <stdio.h>
#include <stdlib.h>
#include "common/remote_def.h"
#include <string.h>
#include "common/common_define.h"

#define NETWORK_EXCEPTION	-1
#define FILE_EXCEPTION		-2
#define UPDATE_SUCCESS		0

#define URL_EXTEND_LEN 128  //len of url extend part

/*
 * update config
 */

//data struct of config update
typedef struct _config_update_data
{
    int url_len;    //len of url
    char *url;      //url of config file
    int up_mode;    //update mode: no update; data update; struct of db update
}config_update_data_t;

// start the thread to update config file
void start_update_cfg_thread(void *param);

// stop the thread of update config file 
void stop_update_cfg_thread();

// the action of thread
int update_cfg_handle(void *param);

void do_config_update();

int curl_download(const char* url,
                   const char* save_path,
                   const char* save_name);

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);

/*
 * update software
 */
// start the thread to update software
void start_update_software_thread(void *param);

// stop the thread of update software 
void stop_update_software_thread();

// the action of thread
int update_software_handle(void *param);

void do_software_update(char *url);


#endif /* UPDATE_MGR_H_ */
