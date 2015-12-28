/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * GeneralAgentHttpKeepConnector.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: GeneralAgentHttpKeepConnector.cpp 5884 2012-07-10 04:11:10Z WuJunjie $
 *
 *  Explain:
 *     -
 *      http 连接器，
 *      保持连接，断开后自动再次连接
 *     -
 *
 *  Update:
 *     2012-07-10 04:11:10  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include <assert.h>
#include <string.h>
#include "GeneralAgentHttpKeepConnector.h"

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#define BUF_LEN 1000
#define DCB_CALLBACK(x) int http_##x##_callback(http_parser *parser, const char *at, size_t length)

DCB_CALLBACK(header_field) {    
#define CODES \
    char str_buf[BUF_LEN];\
    memcpy(str_buf, at, length);\
    str_buf[length] = '\0';

    CODES
    printf("%s : ", str_buf);
    return 0;
}

DCB_CALLBACK(header_value) {
    CODES
    printf("%s\n", str_buf);
    return 0;
}

DCB_CALLBACK(body) {
    CODES
    printf("body : \n%s\n", str_buf);
    return 0;
}

DCB_CALLBACK(url) {
    CODES
    printf("url : %s\n", str_buf);
    return 0;
}

#define CB_CALLBACK(x) int http_##x##_callback(http_parser *parser)
#define TOKEN "---------------------------------- "
CB_CALLBACK(message_begin) {
    printf(TOKEN"message_begin""\n");
    return 0;
}

CB_CALLBACK(message_complete) {
    printf(TOKEN"message_complete""\n");
    
    // 结束了， 初始化，等待下面的数据
    http_parser_init(parser, HTTP_RESPONSE);
    
    return 0;
}

CB_CALLBACK(headers_complete) {
printf(TOKEN"headers_complete""\n");
    return 0;
}
//////////////////////////////////////////////////
CGeneralAgentHttpKeepConnector::CGeneralAgentHttpKeepConnector(ISocketHandler& h, std::string strSocketName)
        :TcpSocket(h)
        //,CEZThread(CGENERALAGENTHTTPKEEPCONNECTOR_THREAD_ATTR)
        ,m_b_connected(false)
{
    // initial connection timeout setting
    SetConnectTimeout(5);

    SetDeleteByHandler();

    SetSockName(strSocketName);
    m_tOnData = 0;


    parser = (http_parser*)malloc(sizeof(http_parser));
    settings = (http_parser_settings*) malloc(sizeof(http_parser_settings));    
    assert(parser);
    assert(settings);
    
    // 仅解析反馈的数据
    http_parser_init(parser, HTTP_RESPONSE);
    
    #define NCB(n) http_##n##_callback
    #define SCB(x) settings->on_##x = NCB(x)
    SCB(url);
    SCB(header_field);
    SCB(header_value);
    SCB(body);
    SCB(message_begin);
    SCB(headers_complete);
    SCB(message_complete);
}


CGeneralAgentHttpKeepConnector::~CGeneralAgentHttpKeepConnector()
{}

CGeneralAgentHttpKeepConnector *CGeneralAgentHttpKeepConnector::Reconnect()
{
    std::auto_ptr<SocketAddress> ad = GetClientRemoteAddress();
    CGeneralAgentHttpKeepConnector *p = new CGeneralAgentHttpKeepConnector(Handler());
    p -> SetDeleteByHandler();
    p -> Open(*ad);
    Handler().Add(p);
    return p;
}


void CGeneralAgentHttpKeepConnector::OnConnectFailed()
{
    CGeneralAgentHttpKeepConnector *p = Reconnect();
    // modify connection timeout setting
    p -> SetConnectTimeout(3);
}


void CGeneralAgentHttpKeepConnector::OnDelete()
{
    if (m_b_connected)
    {
        Reconnect();
    }
}

void CGeneralAgentHttpKeepConnector::SetCloseAndDelete()
{
    TcpSocket::SetCloseAndDelete();
    m_b_connected = false;
}

void CGeneralAgentHttpKeepConnector::OnConnect()
{
    // do not modify, wujj 2012-4-9 15:31:44 begin
    m_b_connected = true;
    // do not modify, wujj 2012-4-9 15:31:44 end
    ////////// add your code here

    printf("CGeneralAgentHttpKeepConnector::OnConnect() - %ld\n", time(NULL));
}
void CGeneralAgentHttpKeepConnector::OnReconnect()
{
    printf("CGeneralAgentHttpKeepConnector::OnReconnect()\n");
}

void CGeneralAgentHttpKeepConnector::OnDisconnect()
{
    printf("CGeneralAgentHttpKeepConnector::OnDisconnect() - %ld\n", time(NULL));
}

void CGeneralAgentHttpKeepConnector::OnConnectTimeout()
{
    printf("CGeneralAgentHttpKeepConnector::OnConnectTimeout()\n");
}

void CGeneralAgentHttpKeepConnector::OnRawData(const char *buf,size_t len)
{
    m_tOnData = time(NULL);
    
    DBG(
    __fline;
    printf("CGeneralAgentTcpSocket::OnRawData:%d\n", len);
    
    std::string strData((char *)buf, len);
    printf("------------------------------------------\n");
    printf("%s", strData.c_str());
    printf("\n------------------------------------------\n");
    );

    int ret = http_parser_execute(parser, settings, buf, len);
    printf("\nhttp_parser_execute ret : %d\n", ret);
    //////////
    //有数据就调用， 另一种方式
    //OnCommand((char *)buf, (unsigned int)len);
    ///////////
}

void CGeneralAgentHttpKeepConnector::OnCommand(char *pCmdData
        , unsigned int ulCmdDataLen)
{
    DBG( printf("CGeneralAgentHttpKeepConnector::OnCommand, len:%d\n", ulCmdDataLen); );
    // 测试用， 关闭自己

    //////////
}

void CGeneralAgentHttpKeepConnector::ThreadProc()
{
}
