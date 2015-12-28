/*
 * md5_tools.cpp
 *
 *  Created on: 2012-2-16
 *      Author: pangt
 */
#include "app.h"
#include <time.h>

void hex_2_string(char hex, char **s)
{
	int a = (u_char) hex / 16;
	int b = (u_char) hex - a * 16;
	//	snprintf(s, "%X%X", a, b);
	snprintf(*s, 3, "%X%X", a, b);
//	sprintf(s, "%X%X", hex);
	//	printf("hex = %d,a = %d,b = %d,s = %s\n", (u_char) hex, a, b, s);
}

int char2dec(char c)
{
	int n = 0;
	char a = 'A';
	char l0 = '0';
	if (c >= a) {
		n = c - a + 10;
	} else {
		n = c - l0;
	}
	return n;
}

char string_2_hex(char s[2])
{
	char r = (char) char2dec(s[0]) * 16 + char2dec(s[1]);
	return r;
}

int mt_encrypt_salt(const char * base_string, char s_encrypt[MD5_LEN],
	char salt[SALT_LEN])
{
	unsigned char s_tmp[MD5_LBLOCK];
	char res[MD5_LBLOCK + SALT_LEN];
	bzero(s_tmp, sizeof(s_tmp));
	int i = 0;
	MD5_CTX c;
	MD5_Init(&c);
	MD5_Update(&c, salt, SALT_LEN);
	MD5_Update(&c, base_string, strlen((const char *) base_string));
	MD5_Final(s_tmp, &c);
	for (i = 0; i < SALT_LEN; i++) {
		res[i] = salt[i];
	//	printf("%d,", (u_char) res[i]);
	}
	printf("\n");
	for (i = 0; i < MD5_LBLOCK; i++) {
		res[i + SALT_LEN] = s_tmp[i];
	}

	//char s[3];
	char * s = (char *)calloc(3,sizeof(char));
	for (i = 0; i < sizeof(res); i++) {
	//			printf("\ni = %d,r = %d\t\n", i + 1, (u_char) res[i]);
hex_2_string(res[i], &s);
		s_encrypt[2 * i] = s[0];
		s_encrypt[2 * i + 1] = s[1];
	}
//	printf("\nwith salt = %s\n", s_encrypt);
free(s);
	return EXIT_SUCCESS;
}

int mt_encrypt(const char * base_string, char s_encrypt[MD5_LEN])
{
	char salt[SALT_LEN];
	int i = 0;
	srand((unsigned) time(NULL));
	for (i = 0; i < sizeof(salt); i++) {
		salt[i] = rand() % 256;
	}

		//printf("--->salt:");
		//for (i = 0; i < SALT_LEN; i++) {
		//	printf("%d,", (u_char) salt[i]);
		//}
		//printf("--->salt\n");

	mt_encrypt_salt(base_string, s_encrypt, salt);
	return EXIT_SUCCESS;
}

//int mt_valid(char s_encrypt[MD5_LEN], const char * pwd, int * is_true)
int mt_validate(const char * s_encrypt, const char * pwd, int * is_true)
{
//	printf("\n0------------->\n");
	int i = 0;
	char res[MD5_LEN / 2];
//	if(NULL == pwd) {
//		printf("pwd in md5 is null\n");
//	}else {
//		printf("pwd in md5 is not null!\n");
//	}
//	printf("\nin md5 pwd : %s----------->\n",pwd);
//
//	printf("1s_encrypt = %s--------------->\n",s_encrypt);
	for (i = 0; i < MD5_LEN; i += 2) {
//	printf("\n10--------------->\n");		
char c_tmp[2] = { s_encrypt[i], s_encrypt[i + 1] };
		//		printf("tmp2 = %d,%d\t", c_tmp[0],c_tmp[1]);
//	printf("\n11--------------->\n");
		res[i / 2] = string_2_hex(c_tmp);
	}

//	printf("\n2--------------->\n");
	char salt[SALT_LEN + 1];
	memcpy(salt, res, SALT_LEN);
	//	for (i = 0; i < SALT_LEN; i++) {
	//		printf("%d,", (u_char) salt[i]);
	//	}
	//	printf("--->salt\n");
	//get whether the pwd after md5 is the encrypt string
	char tmp_res[MD5_LEN + 1];
	bzero(tmp_res, sizeof(tmp_res));
	mt_encrypt_salt(pwd, tmp_res, salt);
//	printf("3--------------->\n");
	if (0 == strcmp(tmp_res, s_encrypt)) {
		//		printf("true\n");
		*is_true = MD5_VALI_TRUE;
	} else {
		//		printf("false\n");
		*is_true = MD5_VALI_FALSE;
	}
//	printf("4--------------->\n");
	return EXIT_SUCCESS;
}
