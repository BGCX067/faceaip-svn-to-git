/*
 * start_up.h
 *
 *  Created on: Feb 17, 2012
 *      Author: SongShanping
 */

#ifndef START_UP_H_
#define START_UP_H_

#include <stdlib.h>

#include <pjlib.h>

extern pj_pool_t * pool_instance;

void start_db();

void end_db();

//start communication model
void start_cm(pj_pool_t *param);

//end communication model
void end_cm();

/*
 * Interface with Host Web
 */

//start host progress
void start(pj_pool_t *param);

//end host progress
void end();

//catch the signal from kill.
void sig_handler(int sig);

#endif /* START_UP_H_ */
