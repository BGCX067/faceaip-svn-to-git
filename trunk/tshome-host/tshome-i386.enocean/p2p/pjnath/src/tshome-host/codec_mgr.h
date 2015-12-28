/*
 * codec_mgr.h
 *
 *  Created on: Feb 17, 2012
 *      Author: SongShanping
 */

#ifndef CODEC_MGR_H_
#define CODEC_MGR_H_

#include "encode/aes_tools.h"
#include "encode/rsa_tools.h"

/*
 * init rsa decode key. it used to decode string sent from terminal.
 */
void init_rsa_decode_key();

/*
 * init rsa encrypt key by rsa key and big num.
 * it used to encrypt string then send to server.
 */
void init_rsa_key(char *rsa_key, char *big_num);

/*
 * destroy rsa encrypt key .
 */
void destroy_rsa_key();

/*
 * destroy rsa decode key
 */
void destroy_rsa_decode_key();

/*
 * init aes key to encrypt msg.
 */
void init_aes_key();

/*
 * destroy aes key
 */
void destroy_aes_key();

#endif /* CODEC_MGR_H_ */
