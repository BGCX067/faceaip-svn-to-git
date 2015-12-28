/*
 * update_mgr.c
 *
 *  Created on: Feb 18, 2012
 *      Author: SongShanping
 */

#include "update_mgr.h"
#include "common/thread.h"
#include "db/ts_db_conn_infos.h"
#include "operation_server_mgr.h"
#include "common/common_define.h"

#include "db/ts_db_tools.h"

#include <curl/curl.h>
#include <curl/easy.h>
#include <string.h>
#include <stdio.h>

#define FILENAME_MAX 512

// config updae data.
config_update_data_t *g_cfg_up_data = NULL;

// to judge the update cfg thread is stop or not.
int is_update_cfg_thread_stop = TRUE;

// to count the re-download times
int retry_cfg_count = 0;

// the url of software-update file. 
char *software_url = NULL;

// to judge the update software thread is stop or not.
int is_update_software_thread_stop = TRUE;

//to count the re-download times
int retry_software_count = 0;

THREAD_MUTEX mux_lock_config_up_flag = PTHREAD_MUTEX_INITIALIZER;

THREAD_MUTEX mux_lock_software_up_flag = PTHREAD_MUTEX_INITIALIZER;

/*
 *curl download
 */

int curl_download(const char* url,
                   const char* save_path,
                   const char* save_name)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    char outfilename[FILENAME_MAX] = {0};
    sprintf(outfilename,"%s/%s",save_path,save_name);
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfilename,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    return (int)res;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

/*
 * update config
 */
void do_config_update()
{
    if (retry_cfg_count > 4){
        printf(">>>> In do_config_update : UPDATE FAILED\n");
        return;
    }
    retry_cfg_count++;
    
    printf(">>>> In do_config_update :re update count = %d\n",retry_cfg_count);
    const char* save_name = "ts_config_version";
    const char* download_url = (const char* )(g_cfg_up_data->url);
    int result = curl_download(download_url, CONFIG_FILE_SAVE_PATH, save_name);

    char path[64] = {0};
    sprintf(path,"%s%s",CONFIG_FILE_SAVE_PATH,save_name);
    
    printf(">>>> In do_config_update : update result = %d\n", result);
    
    if (UPDATE_SUCCESS == result){
        
        printf(">>>> In do_config_update :  UPDATE_SUCCESS\n");
        
        ts_db_tools_parse_xml(path);
        
        
        printf(">>>> In do_config_update :  ts_db_tools_parse_xml OVER\n");

    }else{
        do_config_update();
    }
}

int update_cfg_handle(void *param)
{
    printf(">>>> Into update_cfg_handle(void *param)\n");
    LOCK_MUTEX(&mux_lock_config_up_flag);
    retry_cfg_count = 0;

    do_config_update();
    
    stop_update_cfg_thread();
    UNLOCK_MUTEX(&mux_lock_config_up_flag);

    return 0;
}

void start_update_cfg_thread(void *param)
{
    LOCK_MUTEX(&mux_lock_config_up_flag);
    printf(">>>> Into start_update_cfg_thread: \n");
    config_update_data_t *temp = (config_update_data_t *)param;
    g_cfg_up_data = (config_update_data_t *)malloc(sizeof(config_update_data_t));
    if (NULL == g_cfg_up_data){
        UNLOCK_MUTEX(&mux_lock_config_up_flag);
        return;
    }
    g_cfg_up_data->up_mode = temp->up_mode;
    int len = temp->url_len + URL_EXTEND_LEN + 1;
    g_cfg_up_data->url = (char *)malloc(len);
    if (g_cfg_up_data->url == NULL) {
        free(g_cfg_up_data);
        g_cfg_up_data = NULL;
        UNLOCK_MUTEX(&mux_lock_config_up_flag);
        return;
    }
    memset(g_cfg_up_data->url, 0, len);
    
    char url_extend[URL_EXTEND_LEN] = {0};
    int home_id = 0;
    
    db_get_home_id(&home_id);
    
    int32_t session_id = get_server_session_id();
    int s_id = session_id;
    
    printf(">>>> In start_update_cfg_thread :home_id = %d, sessionId = %d\n",home_id, s_id);
    
    sprintf(url_extend, "&sessionId=%d&type=host&from=c",s_id);
    sprintf(g_cfg_up_data->url,"%s%s",temp->url,url_extend);
    
    
    printf(">>>> In start_update_cfg_thread : FINAL URL = %s\n", g_cfg_up_data->url);

    
    UNLOCK_MUTEX(&mux_lock_config_up_flag);

    THREAD_HANDLE update_cfg_handle_thread = CREATE_THREAD(update_cfg_handle, NULL);
    is_update_cfg_thread_stop = FALSE;
    DETACH_THREAD(update_cfg_handle_thread);
    update_cfg_handle_thread = 0;
}

void stop_update_cfg_thread()
{
    is_update_cfg_thread_stop = TRUE;

    if (NULL != g_cfg_up_data){
        if (NULL != g_cfg_up_data->url) {
            free(g_cfg_up_data->url);
            g_cfg_up_data->url = NULL;
        }
        free(g_cfg_up_data);
        g_cfg_up_data = NULL;
    }
}

/*
 * update software
 */
void start_update_software_thread(void *param)
{
    LOCK_MUTEX(&mux_lock_software_up_flag);
    software_url = (char *)malloc(strlen((char*)param));
    UNLOCK_MUTEX(&mux_lock_software_up_flag);
    
    if (NULL == software_url){
        return;
    }

    THREAD_HANDLE update_soft_handle_thread =
        CREATE_THREAD(update_software_handle, (void*)software_url);

    DETACH_THREAD(update_soft_handle_thread);
    update_soft_handle_thread = 0;
}

void stop_update_software_thread()
{
    if (NULL != software_url){
        free(software_url);
        software_url = NULL;
    }
}

int update_software_handle(void *param)
{
    LOCK_MUTEX(&mux_lock_software_up_flag);
    char *url = (char*)param;
    if (NULL == url){
        UNLOCK_MUTEX(&mux_lock_software_up_flag);
        return -1;
    }

    retry_software_count = 0;

    do_software_update(url);

    free(software_url);
    software_url = NULL;
    UNLOCK_MUTEX(&mux_lock_software_up_flag);

    return 0;
}

void do_software_update(char *url)
{
    if (retry_software_count > 4){
        return;
    }
    retry_software_count++;
    const char* save_name = "ts_software_version";
    const char* download_url = (const char* )url;
    int result = curl_download(download_url, SOFTWARE_FILE_SAVE_PATH, save_name);
    
    char path[64] = {0};
    sprintf(path,"%s%s",CONFIG_FILE_SAVE_PATH,save_name);
    
    if (UPDATE_SUCCESS == result){
        
        ts_db_tools_upd_software(path);

    }else if (NETWORK_EXCEPTION == result){
        do_software_update(url);
    }else if (FILE_EXCEPTION == result){
        do_software_update(url);
    }
}

