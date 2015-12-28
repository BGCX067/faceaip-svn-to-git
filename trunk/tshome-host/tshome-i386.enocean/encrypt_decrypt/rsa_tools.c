/*
 * rsa_tools.c
 *
 *  Created on: 2011-12-28
 *      Author: pangt
 */

#include "rsa_tools.h"
#include <log/ts_log_tools.h>

/**
 * cretae RSA
 */
RSA * rt_create_rsa()
{
	RSA * r = RSA_new();
	r = RSA_generate_key(RSA_LEN, RSA_F4, NULL, NULL);
	return r;
}

/**
 * init RSA
 */
RSA * rt_get_rsa_ins(const char * n, const char * public_key,
	const char * private_key)
{
	//Big number
	BIGNUM *bnn, *bne, *bnd;
	bnn = BN_new();
	bne = BN_new();
	bnd = BN_new();
	BN_hex2bn(&bnn, n);
	BN_hex2bn(&bne, public_key);
	BN_hex2bn(&bnd, private_key);

	//RSA
	RSA *r = RSA_new();
	if (NULL != r) {
		r->n = bnn;
		r->e = bne;
		r->d = bnd;
	}
	return r;
}

/**
 * rsa encrypt
 */
int32_t rt_encrypt(const char * base, char * s_encode, RSA *rsa)
{
	int32_t ret = EXIT_SUCCESS;
	int32_t flen = RSA_size(rsa);

	int32_t flag = EXIT_SUCCESS;
	//	int32_t i = 0;
	//	for (i = 0; i < 3; i++) {
	ret = RSA_public_encrypt(flen, (const u_char*) base, (u_char*) s_encode,
		rsa, RSA_NO_PADDING);
	//	ret = RSA_private_encrypt(flen, (const u_char*) base, (u_char*) s_encode,
	//		rsa, RSA_NO_PADDING);
	if (ret < 0) {
		log_err("c_encode failed!ret = %d, %lx\n", ret, ERR_get_error());
		flag = EXIT_FAILURE;
		//		} else {
		//			flag = EXIT_SUCCESS;
		//			break;
		//		}
	}
	return flag;
}

/**
 * rsa decrypt
 */
int32_t rt_decrypt(const char *s_encrypt, char *s_decrypt, RSA *rsa)
{
	int32_t ret = EXIT_SUCCESS;
	int32_t flen = RSA_size(rsa);
	ret = RSA_private_decrypt(flen, (const u_char *) s_encrypt,
		(u_char *) s_decrypt, rsa, RSA_NO_PADDING);
	//	ret = RSA_public_decrypt(flen, (const u_char *) s_encrypt,
	//		(u_char *) s_decrypt, rsa, RSA_NO_PADDING);
	if (ret < 0) {
		log_err("c_decode failed! %ld\n", ERR_get_error());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

char * rt_get_rsa_publicKey(RSA *r)
{
	return r == NULL ? NULL : BN_bn2hex(r->e);
}

char * rt_get_rsa_bigNum(RSA *r)
{
	return r == NULL ? NULL : BN_bn2hex(r->n);
}

/**
 * destory Rsa
 */
void rt_destory_rsa_ins(RSA *r)
{
	free(r->n);
	free(r->d);
	free(r->e);
	free(r);
}
