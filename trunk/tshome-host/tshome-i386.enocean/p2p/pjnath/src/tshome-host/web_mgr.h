/*
 * web_mgr.h
 *
 *  Created on: Apr 9, 2012
 *      Author: apple
 */

#ifndef WEB_MGR_H_
#define WEB_MGR_H_

#include "common/common_define.h"

#include "dev_ctrl/Conversion.h"
#include <pjlib.h>
/*
 * start the thread to update config file
 */
void start_web_action_thread(pj_pool_t *param);

/*
 * stop the thread of update config file
 */
void stop_web_action_thread();

/*
 * the action of thread
 */
int web_action_handle(void *param);

/*
 * web do update
 */
void web_update();

/*
 * stop host network by web
 */
int stop_host_network_from_web(int open);

#endif /* WEB_MGR_H_ */
