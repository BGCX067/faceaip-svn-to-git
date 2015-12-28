/*
 * md5_tools.h
 *
 *  Created on: 2012-2-16
 *      Author: pangt
 */

#ifndef MD5_TOOLS_H_
#define MD5_TOOLS_H_


//#include <openssl/md5.h>
#include <../include/openssl/md5.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SALT_LEN 12
#define MD5_LEN (MD5_LBLOCK+SALT_LEN)*2
#define MD5_VALI_TRUE 0
#define MD5_VALI_FALSE 1

/**
 * @parm : base_string : the text what you want to encrypt
 * @parm : s_encrypt : the text what after encrypt
 */
int mt_encrypt(const char * base_string, char s_encrypt[MD5_LEN]);

/**
 * @parm : base_string : the text what after encrypt
 * @parm : s_encrypt : the text what you want validate
 * @parm : is_true:MD5_VALI_TRUE : true ,MD5_VALI_FALSE : false
 */
int mt_validate(const char * s_encrypt, const char * pwd, int * is_true);

#endif /* MD5_TOOLS_H_ */

