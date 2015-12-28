#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode/aes_tools.h"
#include "db/ts_db_tools.h"
#include "db/ts_db_protocol_infos.h"
#include "log/ts_log_tools.h"
#include <curl/curl.h>
#include <curl/easy.h>

#define LAST_LEN 2
#define LEN_TYPE "CLL: "
//#define EN_FILE "../../conf_en"
//#define DE_FILE "../conf/conf_de.xml"
#define EN_FILE "../../conf_en"
#define DE_FILE "../../conf_de.xml"
#define MAXLINE 128

int get_last_length(char * head_path,int *last_length) {
	int ret = EXIT_SUCCESS;
		
	FILE *fp = NULL;
	char *cur_line = NULL;	
	char line[MAXLINE] = {0};
	char s_len[LAST_LEN + 1 ] = {0 };

	fp = fopen(head_path,"r");
	if(NULL == fp ) {
		ret = EXIT_FAILURE;
		return ret;
	} else {
		while ((fgets (line, MAXLINE, fp)) != NULL) {
			cur_line = strstr(line,LEN_TYPE);
			if(NULL != cur_line) {
				memcpy(s_len,cur_line + strlen(LEN_TYPE),LAST_LEN);
				*last_length = strtol(s_len,NULL,10);
				break;
			}
		}
	}

	return ret;
}

int main(int argc,char *argv[]) {

	int ret = 0;
	char s_key[BLOCK_SIZE] = { 0} ;
	int last_length = -1;	

	log_open("update_conf");

	if(argc != 3) {
		log_err("Please give 2 params,conf url and aes's key .\n");
		return EXIT_FAILURE;
	}

	if(NULL == argv[1]) { 
		log_err("The url is error!\n");
		return EXIT_FAILURE;
	} else {
		last_length = -1;
		get_last_length(argv[1],&last_length);
	}

	//The last length can't be 0;
	if(last_length <= 0 ) {
		log_err("Get length error,check the file name!\n");
		return EXIT_FAILURE;
	}

	if((NULL == argv[2])||(strlen(argv[2]) < BLOCK_SIZE * 2 )) {
		log_err("Can't get aes key\n");
		return EXIT_FAILURE;
	} 

	//Get aes ins
	aes_encode aes_encode_ins;
	bzero(aes_encode_ins.key,BLOCK_SIZE);
	string_2_hex(argv[2],s_key);
	at_create_key_by_string((u_char *)s_key,&aes_encode_ins);
	ret = at_ed_file(EN_FILE,DE_FILE,aes_encode_ins,TS_AES_DECRYPT,&last_length);
	if(EXIT_SUCCESS != ret ) {
		log_err("Decryt file error!\n");
		return ret;
	}

	ret = ts_db_tools_parse_xml(DE_FILE);
	if(EXIT_SUCCESS != ret ) {
		log_err("Imp error !\n");
		return ret;
	}

	log_close();
	return ret;
}

