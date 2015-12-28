/*
 * md5_tools.h
 *
 *  Created on: 2012-2-16
 *      Author: pangt
 */

#ifndef __MD5_TOOLS_H__
#define __MD5_TOOLS_H__


#include <openssl/md5.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SALT_LEN 12
#define MD5_LEN (MD5_LBLOCK+SALT_LEN)*2
#define MD5_VALI_TRUE 0
#define MD5_VALI_FALSE 1

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @parm : base_string : the text what you want to encrypt
 * @parm : s_encrypt : the text what after encrypt
 */
int32_t mt_encrypt(const char * base_string, char s_encrypt[MD5_LEN]);

/**
 * @parm : base_string : the text what after encrypt
 * @parm : s_encrypt : the text what you want validate
 * @parm : is_true:MD5_VALI_TRUE : true ,MD5_VALI_FALSE : false
 */
int32_t mt_valid(const char *s_encrypt, const char * pwd, int32_t * is_true);

#ifdef __cplusplus
}
#endif

#endif /* MD5_TOOLS_H_ */

