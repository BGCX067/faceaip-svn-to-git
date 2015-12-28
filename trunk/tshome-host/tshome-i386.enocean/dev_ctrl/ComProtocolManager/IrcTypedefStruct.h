
#ifndef CIRCTYPEDEFSTRUCT_H
#define CIRCTYPEDEFSTRUCT_H

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef UINT
#define UINT unsigned int
#endif

#include <string.h>

typedef void (* OnParsedData) (int nType, int nComId, char *strProtocolName, void *pBuf, int nLen);
typedef void (* ToSendComDataByComIndex) (int nComIndex, char * pMsg, int nLen);
typedef void (* ToSendComDataByProtoAddr) (char *strProtocol, int nAddress, char * pMsg, int nLen);


typedef enum
{
	COM_PORT_1 = 0,
	COM_PORT_2,
	COM_PORT_3,
	COM_PORT_4
}Com_Port_Index;

typedef enum
{
	IFRARED_STUDY = 0x01,
	IFRARED_STUDY_QUIT,
	IFRARED_CTRL,
	SERIAL_CTRL,
	SERIAL_PARAM 
} IRC_CMD_CODE;

typedef enum
{
	CTRL_GENENRAL = 0x00,
	CTRL_OPEN,
	CTRL_CLOSE
} IRC_CMD_CODE2;

typedef enum
{
	MSG_STATE_REQUEST = 1,
	MSG_STATE_RESPONSE
} IRC_MSG_STATE;

typedef struct _IRC_Request
{
	BYTE cmd;
	BYTE cmd2;
	BYTE state;
	BYTE address;
	UINT length;
	BYTE data[0];
} IRC_Request, IRC_Response;


// functions
inline int FillIrcRquest(BYTE* buffer, IRC_CMD_CODE cmd, IRC_CMD_CODE2 cmd2, BYTE address, UINT dataLen, BYTE *data)
{
	IRC_Request* pReq = (IRC_Request*) buffer;
	pReq->cmd = (BYTE) cmd;
	pReq->cmd2 = (BYTE) cmd2;
	pReq->state = MSG_STATE_REQUEST;
	pReq->address = (BYTE) address;
	pReq->length = dataLen;
	if(dataLen > 0)
	{
		memcpy(pReq->data, data, dataLen);
	}

	return (sizeof(IRC_Request) + dataLen);
}

inline int FillIrcResponse(BYTE* buffer, IRC_CMD_CODE cmd, IRC_CMD_CODE2 cmd2, BYTE address, UINT dataLen, BYTE *data)
{
	IRC_Response* pRes = (IRC_Response*) buffer;
	pRes->cmd = (BYTE) cmd;
	pRes->cmd2 = (BYTE) cmd2;
	pRes->state = MSG_STATE_RESPONSE;
	pRes->address = (BYTE) address;
	pRes->length = dataLen;
	if(dataLen > 0)
	{
		memcpy(pRes->data, data, dataLen);
	}

	return (sizeof(IRC_Response) + dataLen);
}

#endif
