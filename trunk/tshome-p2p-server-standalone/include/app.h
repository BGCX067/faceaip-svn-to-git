#ifndef APP_H_
#define APP_H_

#include <app/datagram/IPCProtocolDef.h>
#include <app/datagram/remote_def.h>
#include <app/server/types.h>

#include <app/datagram/IPCProtocolManager.h>
#include <app/datagram/report_mgr.h>
#include <pjlib.h>

#include <app/server/ts_server.h>
#include <app/server/auth.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <app/server/turn.h>
#include <app/server/snd_rcv.h>
#include <app/server/pj_server.h>
#include <app/server/mysqlpool.h>
#include <app/server/EasyMysqlConn.h>

#include <app/common/mybyteswap.h>

#ifdef __cplusplus
}
#endif

#endif /*APP_H_*/

