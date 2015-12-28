#ifndef CONSTANTS_H_
#define CONSTANTS_H_

/*
 *for tshome-server.c
 */
#define DEFAULT_PORT   34400             //pj-server port
#define BIND_SERVER_IP "0.0.0.0"//"218.95.39.172"   //ipc-server ip
#define BIND_PORT      11113             //ipc-server port
#define AES_PWD        "tiansuipc"

/*
 * for ipc-server.c
 */
#define BUFFER_SIZE    100               //the ipc receive queue size
#define DATA_LEN       128               //the send buffer size
#define INVALID_VALUE  -1                //invalid value

/*
 *for userdb.c
 */
#define SEND_BUFF_SIZE 512               //the send buffer size
// 第一份sdh不用， 故而将sdh由5改为6 wujj 2012-8-24 13:46:43
#define SDPH_BUFF_SIZE 6                 //the SDPH buffer size
#define SDPL_BUFF_SIZE 1                 //the SDPL buffer size
#define BEGIN_SESSION_ID 100             //session id start value.

/*
 *for TSHOME protocol
 */
#define TSHOME_PROTOCOL_VERSION 1	//protocol version
#endif
