/*ipc_handler.c
 *
 *  Created on: 2012-8-1
 *      Author: sunzq
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "app.h"


static void destroy_handler(ipc_handler *handler);


int ipc_handler_init(const char* secret_key,
                     const unsigned key_len, ipc_handler *handler)

{
    //init handler
    handler->incoming_handler = &ipc_protocol_recv_handler;
    handler->outgoing_handler = &ipc_protocol_send_handler;

    //set callback
    handler->destroy = &destroy_handler;

    //create AES
    at_create_key_by_string((const u_char*)secret_key, &handler->aes_ins);
    //end

    return TS_SUCCESS;
}


static void destroy_handler(ipc_handler *handler)
{
    //free(ipc_srv);
}

