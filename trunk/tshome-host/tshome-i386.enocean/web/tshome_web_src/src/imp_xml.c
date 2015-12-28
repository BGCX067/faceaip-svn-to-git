/*
 * set_serial_info.c
 *
 *  Created on: 2012-3-28
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cgic.h>

#include "db/sqlite3_ctrl.h"
#include "db/ts_db_web_infos.h"
#include "db/ts_db_tools.h"
#include "log/ts_log_tools.h"
#include "common/common_define.h"

#define FILE_LOAD_NAME "fileload"
#define CONF_FILE "conf/conf.xml"
#define BufferLen 1024

int32_t imp_file()
{
	int32_t ret = EXIT_SUCCESS;

	cgiFilePtr file;
	int targetFile;
//	mode_t mode;
	char name[128];
	char fileNameOnServer[64] = { 0 };
//	char contentType[1024];
	char buffer[BufferLen];
//	char *tmpStr = NULL;
	int size;
	int got; //, t;

	char txt[64] = {0};
	cgiFormString("txt", txt, 64);
	fprintf(stderr,"txt = %s\n",txt);

	ret = cgiFormFileName(FILE_LOAD_NAME, name, sizeof(name));
	if (ret != cgiFormSuccess) {
		log_debug_web("could not retrieve filename,err code %d\n", ret);
		goto FAIL;
	}

	cgiFormFileSize(FILE_LOAD_NAME, &size);
	log_debug_web("name = %s,size = %d\n", name, size);

//	//取得文件类型，不过本例中并未使用
//	cgiFormFileContentType("FileUpload", contentType, sizeof(contentType));

	if (cgiFormFileOpen(FILE_LOAD_NAME, &file) != cgiFormSuccess) {
		log_debug_web("could not open the file\n");
		goto FAIL;
	}

	strcpy(fileNameOnServer, "conf/conf.xml");
	//new a file
	targetFile = open(fileNameOnServer, O_RDWR | O_CREAT | O_TRUNC | O_APPEND);
	//	if (targetFile > 0) {
	//		log_debug_web( "could not create the new file,%s\n", fileNameOnServer);
	//		goto FAIL;
	//	}

	//write file
	while (cgiFormFileRead(file, buffer, BufferLen, &got) == cgiFormSuccess) {
		if (got > 0) {
			write(targetFile, buffer, got);
		}
	}
	cgiFormFileClose(file);
	close(targetFile);
	goto END;

	FAIL: log_debug_web("Failed to upload\n");
	return EXIT_FAILURE;

	END: log_debug_web("File \"%s\" has been uploaded\n", fileNameOnServer);
	return EXIT_SUCCESS;

	return ret;
}

int32_t cgiMain(void)
{
	int ret = EXIT_SUCCESS;
	log_open("imp_xml.c");

	fprintf(cgiOut, "Content-Type:text/html;charset=utf-8\n\n");
//	cgiHeaderContentType("text/html");

	ret = imp_file();
	if (EXIT_SUCCESS == ret) {
		int32_t is_running = TS_NO_RUNNING;
		ts_is_running(TS_MONITOR_NAME, &is_running);
		//kill host
		if (TS_RUNNING == is_running) {
			stop_pro();
		}
		ret = ts_db_tools_parse_xml(CONF_FILE);

		//start host
		if (TS_RUNNING == is_running) {
			start_pro();
		}
	}


	if (EXIT_SUCCESS != ret) {
		fprintf(cgiOut, "导入有误，请检查配置文件\n");
	} else {
		fprintf(cgiOut, "导入成功\n");
	}

//close db
	ret = sc_close();
	if (ret != SQLITE_OK) {
		return EXIT_FAILURE;
	}

	log_close();
	return ret;
}

