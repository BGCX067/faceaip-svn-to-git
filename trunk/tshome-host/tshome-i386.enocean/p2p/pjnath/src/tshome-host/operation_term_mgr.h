/*
 * operation_term_mgr.h
 *
 *  Created on: Feb 9, 2012
 *      Author: SongShanping
 */

#ifndef OPERATION_TERM_MGR_H_
#define OPERATION_TERM_MGR_H_

#include "report_mgr.h"
#include "codec_mgr.h"
#include "start_up.h"

/*
 * get the session id of new terminal when it add to the terminal list.
 */
int get_new_terminal_session_id();

/*
 * deal with the action of ask ras secret key from terminal.
 */
int term_ask_rsa_secret_handle(unsigned k,
								unsigned comp_id,
								PROTOCOL_TYPE net_type);

/*
 * deal with the action of pass aes secret key from terminal.
 */
int term_pass_aes_secret_handle(unsigned k,
								unsigned comp_id,
                                PROTOCOL_TYPE net_type,
                                char *aes_key_buff);
/*
 * deal with the log in action from terminal.
 */
int term_login_handle(unsigned k,
						unsigned comp_id,
						PROTOCOL_TYPE net_type,
						login_req_t * req);

/*
 * deal with the log out action from terminal.
 */
int term_logout_handle(unsigned k,
						unsigned comp_id,
						PROTOCOL_TYPE net_type,
						int32_t session_id);

#endif /* OPERATION_TERM_MGR_H_ */
