/*
 * IPCProtocolManager.h
 *
 *  Created on: 2012-2-7
 *      Author: sunzq
 */

#ifndef IPCPROTOCOLMANAGER_H_
#define IPCPROTOCOLMANAGER_H_

#include <app/datagram/IPCProtocolDef.h>
#include <app/server/types.h>
#include <app/common/aes_tools.h>
/*
 *@function: handle a message received  interface functions
 * -input@recv_buff: received buffer
 * -output@structs: to save data
 * @return : success or failed
 */
 #ifdef __cplusplus
extern "C" {
#endif

u_char ipc_protocol_recv_handler(aes_encode aes, const u_char *recv_buff, void **structs);

/*
 *@function: handle a message received  interface functions
 * -output@send_buff: send buffer
 * -input@structs: data saved
 * - input@structs_type: indicate a current structs
 * @return : success or failed
 */
int ipc_protocol_send_handler(aes_encode aes,
				u_char *send_buff,
				const void *structs,
				const IPC_FUN_CODE fun);
#ifdef __cplusplus
}
#endif

#endif /* IPCPROTOCOLMANAGER_H_ */
