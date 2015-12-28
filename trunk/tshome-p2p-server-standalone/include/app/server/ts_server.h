#ifndef TS_SERVER_H_
#define TS_SERVER_H_

//#include "app.h"
#include "tshome_typedef.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 *create the user container.
*/
int ts_udb_create();

/******************public API for ipc_handler*******************/
int ipc_handler_init(const char* secret_key,
                      const unsigned key_len, ipc_handler *handler);



/******************public API for tsp_handler*******************/
pj_status_t tsp_handler_init(pj_turn_srv *pj_srv);

#ifdef __cplusplus
}
#endif

#endif /*TS_SERVER_H_*/

