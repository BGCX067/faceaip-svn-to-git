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
#ifndef __SND_RCV_H__
#define __SND_RCV_H__

#include <pjnath.h>
#include "app.h"

pj_status_t init_turn_allocation_key (pj_turn_allocation_key *dst_key,
                                      const char *dst_addr,
                                      pj_uint16_t port);

pj_status_t send_data_to_client (pj_turn_srv *srv, 
                                 const unsigned char *data,
                                 unsigned data_len,
                                 //pj_bool_t retransmit,
                                 const char *dst_addr, 
                                 pj_uint16_t port);

pj_status_t deal_with_payload (pj_turn_srv *srv,
                               pj_uint8_t *rx_data,
                               int rx_data_len,
                               char *username,
                               const pj_sockaddr_t *src_addr,
                               unsigned src_addr_len,
                               pj_uint8_t *result,
                               unsigned *size);

#endif
