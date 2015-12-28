/*
 * md5_tools.cpp
 *
 *  Created on: 2012-2-16
 *      Author: pangt
 */

#include "md5_tools.h"
#include <time.h>
#include <log/ts_log_tools.h>

static void hex_2_string(char hex, char **s)//char s[3])
{
	int32_t a = (u_char) hex / 16;
	int32_t b = (u_char) hex - a * 16;
	sprintf(*s, "%X%X", a, b);
	//	printf("hex = %d,a = %d,b = %d,s = %s\n", (u_char) hex, a, b, s);
}

static int32_t char2dec(char c)
{
	int32_t n = 0;
	char a = 'A';
	char l0 = '0';
	if (c >= a) {
		n = c - a + 10;
	} else {
		n = c - l0;
	}
	return n;
}

static char string_2_hex(const char *s, int32_t len)
{
	char r = (char) char2dec(s[0]) * 16 + char2dec(s[1]);
	return r;
}

int32_t mt_encrypt_salt(const char * base_string, char s_encrypt[MD5_LEN],
	char salt[SALT_LEN])
{
	unsigned char s_tmp[MD5_LBLOCK];
	char res[MD5_LBLOCK + SALT_LEN];
	bzero(s_tmp, sizeof(s_tmp));
	int32_t i = 0;
	MD5_CTX c;
	MD5_Init(&c);
	MD5_Update(&c, salt, SALT_LEN);
	MD5_Update(&c, base_string, strlen((const char *) base_string));
	MD5_Final(s_tmp, &c);
	for (i = 0; i < SALT_LEN; i++) {
		res[i] = salt[i];
		//		printf("%d,", (u_char) res[i]);
	}
	//	printf("\n");
	for (i = 0; i < MD5_LBLOCK; i++) {
		res[i + SALT_LEN] = s_tmp[i];
	}

	//	char s[3];
	char *s = (char *) (calloc(3, sizeof(char)));
	for (i = 0; i < sizeof(res); i++) {
		//		printf("i = %d,r = %d\t", i, (u_char) res[i]);
		hex_2_string(res[i], &s);
		s_encrypt[2 * i] = s[0];
		s_encrypt[2 * i + 1] = s[1];
	}
	//	printf("\nwith salt = %s\n", s_encrypt);
	free(s);
	return EXIT_SUCCESS;
}

int32_t mt_encrypt(const char * base_string, char s_encrypt[MD5_LEN])
{
	char salt[SALT_LEN];
	int32_t i = 0;
	srand((unsigned) time(NULL));
	for (i = 0; i < sizeof(salt); i++) {
		salt[i] = rand() % 256;
	}

	mt_encrypt_salt(base_string, s_encrypt, salt);
	return EXIT_SUCCESS;
}

int32_t mt_valid(const char *s_encrypt, const char * pwd, int32_t * is_true)
{
	int32_t i = 0;
	char res[MD5_LEN / 2];
	for (i = 0; i < MD5_LEN; i += 2) {
		char c_tmp[2] = { s_encrypt[i], s_encrypt[i + 1] };
		res[i / 2] = string_2_hex(c_tmp, 2);
	}

	char salt[SALT_LEN + 1];
	memcpy(salt, res, SALT_LEN);
	//get whether the pwd after md5 is the encrypt string
	char tmp_res[MD5_LEN + 1];
	bzero(tmp_res, sizeof(tmp_res));
	mt_encrypt_salt(pwd, tmp_res, salt);
	if (0 == strcmp(tmp_res, s_encrypt)) {
		//		printf("true\n");
		*is_true = MD5_VALI_TRUE;
	} else {
		//		printf("false\n");
		*is_true = MD5_VALI_FALSE;
	}
	return EXIT_SUCCESS;
}
