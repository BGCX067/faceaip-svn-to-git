#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "log/ts_log_tools.h"

#define TS_NULL_FILE "null"
#define TS_URL_LEN 256
#define TS_CMD_LEN 128 
#define TS_PATH_LEN 128

/*size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}*/

/**
*download file
*/
int download_file(char *url,char *path,char *head_path) {
	int ret = EXIT_SUCCESS;
	CURL *curl;
	FILE *fp = NULL;
	FILE *fp_head = NULL;
	CURLcode res;
	curl = curl_easy_init();
	if (NULL != curl) {
		fp = fopen(path,"wb");
		if (NULL == fp) {
			fprintf (stderr,"Open %s failed\n", path);
			return 1;
		}
		curl_easy_setopt(curl, CURLOPT_URL, url);
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		if(0 != strcmp(TS_NULL_FILE,head_path)) {
			fp_head = fopen(head_path,"wb");
			if (NULL == fp_head) {
				printf ("Open %s failed\n", head_path);
				return 1;
			}
			curl_easy_setopt(curl, CURLOPT_WRITEHEADER, fp_head);
		}
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
		if(NULL != fp_head) {
			fclose(fp_head);
		}
		ret = (int)res;
	} else {
		ret = EXIT_FAILURE;
	}
	return ret;
}

int main(int argc,char *argv[]) {
	int ret = EXIT_SUCCESS;
	char conf_url[TS_URL_LEN] = { 0 };
	char path[TS_URL_LEN] = { 0 };
	char head_path[TS_URL_LEN] = { 0 };

	log_open("update download");

	//argv[1] : url, argv[2] : path
	if((argc != 3) &&(argc != 4)) {
		fprintf(stderr,"The arg is not right .Arg1 : url ,arg2 : dst path.arg3 : head path\n");
		ret = EXIT_FAILURE;
		return ret;
	}

	//get urls
	strcpy(conf_url , argv[1]);
	strcpy(path , argv[2]);
	if(NULL != argv[3]) {
		strcpy(head_path,argv[3]);
	} else {
		strcpy(head_path,TS_NULL_FILE);
	}

	//download conf url
	if(0 != strcmp(TS_NULL_FILE , conf_url)) {
		ret = download_file(conf_url , path,head_path);
		if(EXIT_SUCCESS != ret) {
			return EXIT_FAILURE;
		}
	}

	log_close();
	return ret;
}
