/*
 * aes_tools.h
 *
 *  Created on: 2011-12-22
 *      Author: pangt
 */

#ifndef __AES_TOOLS_H__
#define __AES_TOOLS_H__

#include <openssl/aes.h>
#include <sys/types.h>

#define BLOCK_SIZE AES_BLOCK_SIZE
#define ts_random(x) (rand()%x)
#define AES_KEY_BITS 128
#define TS_AES_ENCRYPT 0
#define TS_AES_DECRYPT 1


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _ts_aes_encode
{
	AES_KEY encrypt_aes;
	AES_KEY decrypt_aes;
	unsigned char key[BLOCK_SIZE];
} aes_encode;

/**
 * create key
 */
int32_t at_create_key(aes_encode *aes_ins);

/**
 * create aes key by string
 */
int32_t at_create_key_by_string(const u_char *s, aes_encode *aes_ins);


/**
 * get encrypt length
 */
int32_t at_get_encrypt_len_from_strlen(int32_t str_len);

/**
 * get encrypt length
 */
int32_t at_get_encrypt_len(const char * base_string);

/**
 * encrypt
 */
int16_t at_encrypt(const char * base_string, char *s_encrypt, int32_t len,
	aes_encode aes_encode_ins);

/**
 * decrtypt
 */
int16_t at_decrypt(char* s_encrypt, char * s_decrypt, int32_t len,
	aes_encode aes_encode_ins);

/*
* encrypt file
*/
int16_t at_ed_file(const char *file_name,const char *res_file_name,aes_encode aes_encode_ins,int8_t ed_type,int32_t *last_read);

#ifdef __cplusplus
}
#endif

#endif /* AES_TOOLS_H_ */
