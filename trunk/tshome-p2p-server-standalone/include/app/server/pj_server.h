/*
 * pj_server.h
 *
 *  Created on: 2012-3-9
 *      Author: sunzq
 */

#ifndef TS_PJ_SERVER_H_
#define TS_PJ_SERVER_H_

#define REALM "pjsip.org"
#include "app.h"
/*
 *create a pjsip server.
 *@param local_port                //local listener port.
 *@output param
*/
int ts_pj_server_create(const unsigned int local_port, 
			pj_turn_srv **srv);

/*
 *start the pjsip server.
*/
int ts_pj_server_start(void **arg);

/*
 *stop the pjsip server.
*/
int ts_pj_server_stop(void *arg);

/*
 *destroy the pjsip server.
*/
int ts_pj_server_destroy(pj_turn_srv *srv);

#endif /* TS_PJ_SERVER_H_ */
