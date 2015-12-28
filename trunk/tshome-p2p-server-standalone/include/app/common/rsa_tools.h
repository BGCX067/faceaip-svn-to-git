/*
 * rsa_tools.h
 *
 *  Created on: 2011-12-28
 *      Author: pangt
 */

#ifndef RSA_TOOLS_H_
#define RSA_TOOLS_H_


#include <stdio.h>
#include <stdlib.h>
//#include <openssl/rsa.h>
//#include <openssl/err.h>
#include <../include/openssl/rsa.h>
#include <../include/openssl/err.h>
#include <sys/types.h>


#define RSA_LEN 1024
#define RSA_TEXT_LEN 128
#ifdef __cplusplus
extern "C" {
#endif

/**
 * cretae RSA
 */
RSA * rt_create_rsa();

/**
 * init RSA
 */
RSA * rt_get_rsa_ins(const char * n, const char * public_key,
		const char * private_key);

/**
 * rsa encrypt
 */
int rt_encrypt(const char * base, char * s_encode, RSA *r);

/**
 * rsa decrypt
 */
int rt_decrypt(const char *s_encrypt, char *s_decrypt, RSA *r);

/**
 * destory Rsa
 */
void rt_destory_rsa_ins(RSA *r);

/**
 * get rsa public key
 */
char * rt_get_rsa_publicKey(RSA *r);

/**
 * get rsa big num
 */
char * rt_get_rsa_bigNum(RSA *r);

#ifdef __cplusplus
}
#endif

#endif /* RSA_TOOLS_H_ */

