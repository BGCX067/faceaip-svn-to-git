/* $Id: send.c 3553 2011-05-05 06:14:19Z nanang $ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */
#include "app.h"

#define REALM		"pjsip.org"
//#define TURN_PORT	PJ_STUN_TURN_PORT
#define TURN_PORT	34780
#define LOG_LEVEL	4


pj_status_t init_turn_allocation_key (pj_turn_allocation_key *dst_key,
                                      const char *dst_addr,
                                      pj_uint16_t port)
{
    pj_status_t status;

    PJ_ASSERT_RETURN(dst_key && dst_addr, PJ_EINVAL);

    /* Convert the dst_addr from char to str */
    pj_str_t dst_addr_str = pj_str((char *)dst_addr);

    /* Init turn allocation key */
    status = pj_sockaddr_init(pj_AF_INET(), &dst_key->clt_addr, &dst_addr_str, port);
    if (status != PJ_SUCCESS) {
        return status;
    }
    dst_key->tp_type = PJ_TURN_TP_UDP;

    return PJ_SUCCESS;
}

pj_status_t send_data_to_client (pj_turn_srv *srv, 
                                 const unsigned char *data,
                                 unsigned data_len,
                                 //pj_bool_t retransmit,
                                 const char *dst_addr, 
                                 pj_uint16_t port)
{
    pj_turn_allocation *alloc = NULL;
    pj_stun_session *sess;
    pj_status_t status;
    pj_turn_allocation_key dst_key;

    PJ_ASSERT_RETURN(srv && data && dst_addr, PJ_EINVAL);

    status = init_turn_allocation_key (&dst_key, dst_addr, port);
    if (status != PJ_SUCCESS) {
        goto on_error;
    }
    pj_hash_iterator_t itbuf, *it;

    pj_lock_acquire(srv->core.lock);
    if (srv->tables.alloc) {
        it = pj_hash_first(srv->tables.alloc, &itbuf);
        while (it) {
            alloc = (pj_turn_allocation *)
                    pj_hash_this (srv->tables.alloc, it);
            if(pj_sockaddr_cmp(&alloc->hkey.clt_addr, &dst_key.clt_addr) == 0)
                break;
            it = pj_hash_next(srv->tables.alloc, it);
        }
    } 
    //find the alloc according to dst key 
    //pj_lock_acquire(srv->core.lock);
    //alloc = (pj_turn_allocation*)
    //         pj_hash_get(srv->tables.alloc, &dst_key, sizeof(dst_key), NULL);
    pj_lock_release(srv->core.lock);

    if (alloc) {
        pj_stun_tx_data *tdata = NULL;

        sess = alloc->sess;
        //PJ_ASSERT_ON_FAIL(alloc->sess->state==PJ_TURN_STATE_READY, return);

        //Create a bare REFRESH request
    //    pj_lock_acquire(sess->lock);
        status = pj_stun_session_create_srv_req (sess, PJ_STUN_REFRESH_REQUEST,
                                             PJ_STUN_MAGIC, NULL, &tdata);

        if (status != PJ_SUCCESS) {
            printf ("Create request failed\n");
            //pj_lock_release(sess->lock);
            goto on_error;
        }

        // MUST include REQUESTED-TRANSPORT attribute
        pj_stun_msg_add_uint_attr (tdata->pool, tdata->msg,
                                   PJ_STUN_ATTR_REQ_TRANSPORT,
                                   PJ_STUN_SET_RT_PROTO(PJ_TURN_TP_UDP));
        //Add lifetime
     //   pj_stun_msg_add_uint_attr (tdata->pool, tdata->msg,
      //                             PJ_STUN_ATTR_LIFETIME, 300);

        //Add data
        pj_stun_msg_add_binary_attr (tdata->pool, tdata->msg,
                                   PJ_STUN_ATTR_DATA,
                                   (const pj_uint8_t *)data, data_len);

        //Send request
        status = pj_stun_session_send_msg (sess, NULL, PJ_FALSE,
                                           PJ_TRUE/*retransmit*/, &alloc->hkey.clt_addr,
                                           pj_sockaddr_get_len(&alloc->hkey.clt_addr),
                                           tdata);

        if (status != PJ_SUCCESS) {
            //pj_lock_release(sess->lock);
            goto on_error;
        }

     //   pj_lock_release(sess->lock);
        return PJ_SUCCESS;
    }

    status = !PJ_SUCCESS;
on_error:
    return status;
}


pj_status_t send_data_to_client2 (pj_turn_srv *srv, 
                                 const pj_uint8_t *data,
                                 unsigned data_len,
                                 const pj_sockaddr_t *src_addr,
                                 unsigned src_addr_len )
{
    pj_turn_allocation *alloc = NULL;
    pj_stun_session *sess;
    pj_status_t status;
    pj_turn_allocation_key dst_key;

/*    PJ_ASSERT_RETURN(srv && data && dst_addr, PJ_EINVAL);

    status = init_turn_allocation_key (&dst_key, dst_addr, port);
    if (status != PJ_SUCCESS) {
        goto on_error;
    }
*/
    //find the alloc according to dst key 
    pj_hash_iterator_t itbuf, *it;

    pj_lock_acquire(srv->core.lock);
    if (srv->tables.alloc) {
        it = pj_hash_first(srv->tables.alloc, &itbuf);
        while (it) {
            alloc = (pj_turn_allocation *)
                    pj_hash_this (srv->tables.alloc, it);
            if( pj_sockaddr_cmp(&alloc->hkey.clt_addr, src_addr) != 0)
                break;
            it = pj_hash_next(srv->tables.alloc, it);
        }
    } else
        printf ("tables.alloc has no member\n");

    pj_lock_release(srv->core.lock);



/*    pj_lock_acquire(srv->core.lock);
    alloc = (pj_turn_allocation*)
             pj_hash_get(srv->tables.alloc, &dst_key, sizeof(dst_key), NULL);
    pj_lock_release(srv->core.lock);
*/
    if (alloc) {
        pj_stun_tx_data *tdata = NULL;

        sess = alloc->sess;
        //PJ_ASSERT_ON_FAIL(alloc->sess->state==PJ_TURN_STATE_READY, return);

        //Create a bare REFRESH request
    //    pj_lock_acquire(sess->lock);
        status = pj_stun_session_create_srv_req (sess, PJ_STUN_REFRESH_REQUEST,
                                             PJ_STUN_MAGIC, NULL, &tdata);

        if (status != PJ_SUCCESS) {
            printf ("Create request failed\n");
            //pj_lock_release(sess->lock);
            goto on_error;
        }

        // MUST include REQUESTED-TRANSPORT attribute
        pj_stun_msg_add_uint_attr (tdata->pool, tdata->msg,
                                   PJ_STUN_ATTR_REQ_TRANSPORT,
                                   PJ_STUN_SET_RT_PROTO(PJ_TURN_TP_UDP));
        //Add lifetime
     //   pj_stun_msg_add_uint_attr (tdata->pool, tdata->msg,
      //                             PJ_STUN_ATTR_LIFETIME, 300);

        //Add data
        pj_stun_msg_add_binary_attr (tdata->pool, tdata->msg,
                                   PJ_STUN_ATTR_DATA,
                                   (const pj_uint8_t *)data, data_len);

        //Send request
        status = pj_stun_session_send_msg (sess, NULL, PJ_TRUE,
                                           1, &alloc->hkey.clt_addr,
                                           pj_sockaddr_get_len(&alloc->hkey.clt_addr),
                                           tdata);

        if (status != PJ_SUCCESS) {
            //pj_lock_release(sess->lock);
            goto on_error;
        }

     //   pj_lock_release(sess->lock);
        return PJ_SUCCESS;
    }

on_error:
    return status;
}


pj_status_t deal_with_payload (pj_turn_srv *srv,
                               pj_uint8_t *rx_data,
                               int rx_data_len,
                               char *username,
                               const pj_sockaddr_t *src_addr,
                               unsigned src_addr_len,
                               pj_uint8_t *result,
                               unsigned *size)
{
    static int i=0;
    static char sdp_buf[2][1000];

    PJ_ASSERT_RETURN(rx_data && username && result, PJ_EINVAL);

    /* This is client candicates info*/
    if ( pj_memcmp((char *)rx_data, "SDP:", 4) == 0 ) {
        pj_memcpy (sdp_buf[i], (char *)(rx_data), rx_data_len);
        printf ("\n%s\n", sdp_buf[i]);
        /* Copy the peer SDP Info */
        if (i == 1) {
            *size = strlen(sdp_buf[0]) + 1;
            pj_memcpy (result, sdp_buf[0], *size);
            send_data_to_client2(srv, rx_data, rx_data_len, src_addr, src_addr_len );
        } else {
            *size = 0;
        }
        i++;
    } else if ( pj_memcmp((char *)rx_data, "CMD:", 4) == 0) {
    /* This is client CMD info */
        //pj_memcpy (sdp_buf[i], (char *)(rx_data + 5), rx_data_len - 5);
    }
    return PJ_SUCCESS;
}

