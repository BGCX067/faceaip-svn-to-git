/*
 * codec_mgr.c
 *
 *  Created on: Feb 17, 2012
 *      Author: SongShanping
 */

#include "codec_mgr.h"
#include <string.h>

// aes key to encrypt msg.
aes_encode *g_aes_key = NULL;

// ras key to encrypt msg.
RSA * g_rsa_key = NULL;

// ras key to decode msg
RSA * g_rsa_decode_key = NULL;

void init_rsa_decode_key()
{
    g_rsa_decode_key = rt_create_rsa();
}

void init_rsa_key(char *rsa_key, char *big_num)
{
    if (NULL == rsa_key || NULL == big_num){
        return;
    }
    
    printf(">>>> rsa_key=%s , big_num=%s", rsa_key, big_num);

    destroy_rsa_key();

    g_rsa_key = rt_get_rsa_ins(big_num, rsa_key, NULL);
}

void destroy_rsa_key()
{
    if (NULL != g_rsa_key){
        rt_destory_rsa_ins(g_rsa_key);
        g_rsa_key = NULL;
    }
}

void destroy_rsa_decode_key()
{
    if (NULL != g_rsa_decode_key){
        rt_destory_rsa_ins(g_rsa_decode_key);
        g_rsa_decode_key = NULL;
    }
}

void init_aes_key()
{
    destroy_aes_key();

    g_aes_key = (aes_encode *)malloc(sizeof(aes_encode));
    if (NULL == g_aes_key){
        return;
    }
    memset(g_aes_key, 0, sizeof(aes_encode));
    at_create_key(g_aes_key);
}

void destroy_aes_key()
{
    if (NULL != g_aes_key){
        free(g_aes_key);
        g_aes_key = NULL;
    }
}

