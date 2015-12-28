//
//  KNXException.h
//  tsbus_knx
//
//  Created by kyy on 11-5-21.
//  Copyright 2011 Nanjing TIANSU Automation Control System Co., Ltd. All rights reserved.
//

#ifndef _KNXEXCEPTION_H
#define _KNXEXCEPTION_H

#include <string.h>

	typedef enum
	{
		KNX_ERR_NO = 0,
		KNX_ERR_UNKNOWN = 130,
		KNX_ERR_INPUT,
		KNX_ERR_MULTIINIT,
		KNX_ERR_MULTITHREAD,
		KNX_ERR_UNIINIT,
		KNX_ERR_UNITHREAD,
		KNX_ERR_DPTCONVERSION,
		KNX_ERR_BUSY,
		KNX_ERR_INNER,
		KNX_ERR_SENDINIT,
		KNX_ERR_HANDLEINIT,
		KNX_ERR_SEND,
		KNX_ERR_RECV,
		KNX_ERR_CALLBACK,
		KNX_ERR_TIMEOUT,
		KNX_ERR_BREATHTHREAD,
		KNX_ERR_DISCONNECTED,
		KNX_ERR_GATEWAYSTATUS,
		KNX_ERR_OTHER
	}KNX_EXCEPTION_ID;
	
	class CKNXException
	{
	public:
		virtual ~CKNXException() {};
		virtual KNX_EXCEPTION_ID GetExceptionId() = 0;//{return KNX_ERR_UNKNOW;}
		char strValue[32];
		
		virtual char* GetExceptionString()
		{
			memset(strValue, 0, 32);
			switch (GetExceptionId())
			{
				case KNX_ERR_INPUT:
					strcpy(strValue, "input error");
					break;
				case KNX_ERR_MULTIINIT:
					strcpy(strValue, "multicast init error");
					break;
				case KNX_ERR_MULTITHREAD:
					strcpy(strValue, "multiThread error");
					break;
				case KNX_ERR_UNIINIT:
					strcpy(strValue, "unicast init error");
					break;
				case KNX_ERR_UNITHREAD:
					strcpy(strValue, "uniThread error");
					break;
				case KNX_ERR_DPTCONVERSION:
					strcpy(strValue, "DPT convertion error");
					break;
				case KNX_ERR_BUSY:
					strcpy(strValue, "busy");
					break;
				case KNX_ERR_INNER:
					strcpy(strValue, "inner error");
					break;
				case KNX_ERR_SENDINIT:
					strcpy(strValue, "sendThread init error");
					break;
				case KNX_ERR_HANDLEINIT:
					strcpy(strValue, "handleThread init error");
					break;
				case KNX_ERR_SEND:
					strcpy(strValue, "send error");
					break;
				case KNX_ERR_RECV:
					strcpy(strValue, "recv error");
					break;
				case KNX_ERR_CALLBACK:
					strcpy(strValue, "callback null");
					break;
				case KNX_ERR_TIMEOUT:
					strcpy(strValue, "timeout");
					break;
				case KNX_ERR_BREATHTHREAD:
					strcpy(strValue, "breathThread error");
					break;
				case KNX_ERR_DISCONNECTED:
					strcpy(strValue, "disconnected");
					break;
				case KNX_ERR_GATEWAYSTATUS:
					strcpy(strValue, "gateway status error");
					break;
				case KNX_ERR_OTHER:
					strcpy(strValue, "other error");
					break;
				default:
					strcpy(strValue, "Unknown error");
					break;
			}
			return strValue;
		}
	};
	
	class CKNXUnknownException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_UNKNOWN;
		}
	};
	
	class CKNXInputException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_INPUT;
		}
	};
	
	class CKNXMultiInitException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_MULTIINIT;
		}
	};
	
	class CKNXMultiThreadException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_MULTITHREAD;
		}
	};
	
	class CKNXUniInitException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_UNIINIT;
		}
	};
	
	class CKNXUniThreadException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_UNITHREAD;
		}
	};
	
	class CKNXDPTConvertException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_DPTCONVERSION;
		}
	};
	
	class CKNXBusyException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_BUSY;
		}
	};
	
	class CKNXInnerException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_INNER;
		}
	};
	
	class CKNXSendInitException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_SENDINIT;
		}
	};
	
	class CKNXHandleInitException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_HANDLEINIT;
		}
	};

	class CKNXSendException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_SEND;
		}
	};
	
	class CKNXRecvException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_RECV;
		}
	};
	
	class CKNXCallbackException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_CALLBACK;
		}
	};
	
	class CKNXTimeoutException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_TIMEOUT;
		}
	};
	
	class CKNXBreathThreadException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_BREATHTHREAD;
		}
	};
	
	class CKNXDisConnectedException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_DISCONNECTED;
		}
	};	
	
	class CKNXGatewayStatusException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_GATEWAYSTATUS;
		}
	};	
	
	class CKNXOtherException : public CKNXException
	{
	public:
		virtual KNX_EXCEPTION_ID GetExceptionId()
		{
			return KNX_ERR_OTHER;
		}
	};	

#endif
