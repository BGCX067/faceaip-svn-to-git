/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.cpp - _explain_
 *
 * Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
 *
 * $Id: TestMan.cpp 0001 2012-5-8 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     TeHomeMsg -- Telecom eHome Msg
 *
 *  Update:
 *     2012-5-8 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#include "TestMan.h"

#include "../SmsStack.h"

#ifndef __trip
	#define __trip printf("-W-%d::%s(%d)\n", (int)time(NULL), __FILE__, __LINE__);
#endif
#ifndef __fline
	#define __fline printf("%s(%d)--", __FILE__, __LINE__);
#endif

CPPUNIT_TEST_SUITE_REGISTRATION( CTestMan );

void CTestMan::setUp()
{}
void CTestMan::tearDown()
{}

void CTestMan::testSmsStackParse()
{
    CSmsStack __CSmsStack;

    //std::cout<< endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    {
        std::string strValue = "{\"name\":\"json\",\"age\":\"123\",\"array\":[{\"cpp\":\"jsoncpp\"},{\"java\":\"jsoninjava\"},{\"php\":\"support\"}]}";
        std::string document;

        std::string javaPasedValue;

        __CSmsStack.Parse(strValue);
        __CSmsStack.ToString(document);
        //std::cout << document << std::endl;


        // std::cout << "__CSmsStack.GetValue()[\"name\"].asString(): " << __CSmsStack.GetValue()["name"].asString() << std::endl;
        // std::cout << "__CSmsStack.GetValue()[\"age\"].asString(): " << __CSmsStack.GetValue()["age"].asString() << std::endl;
        CPPUNIT_ASSERT( __CSmsStack.GetValue()["name"].asString() == "json" );
        CPPUNIT_ASSERT( __CSmsStack.GetValue()["age"].asString() == "123" );

        // find the value in array
        const Json::Value arrayObj = __CSmsStack.GetValue()["array"];
        for (unsigned int i = 0; i < arrayObj.size(); i++)
        {
            if (!arrayObj[i].isMember("java"))
            {
                continue;
            }

            //std::cout << "__CSmsStack.GetValue()[\"array\"][\"cpp\"]: " << arrayObj[i]["java"].asString() << std::endl;
            javaPasedValue = arrayObj[i]["java"].asString();
            CPPUNIT_ASSERT( arrayObj[i]["java"].asString() == "jsoninjava" );

        }
    }
}

void CTestMan::testEncodeCAckMsgSimple()
{
    CSmsStack __CSmsStack;

    //    std::cout<< endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    //    {
    //        __CSmsStack.Encode();
    //        std::cout << __CSmsStack.ToString() << std::endl;
    //    }

    std::string result = "100";

    //std::cout<< endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    {
        CAckMsgSimple Msg;
        Msg.result = result;
        Msg.reason = "no such msg";
        //std::string strEncoded = "{\n   \"reason\" : \"no such msg\",\n   \"result\" : \"100\"\n}\n";
        std::string strEncoded = "{ \"reason\" : \"no such msg\", \"result\" : \"100\" }\n";

        __CSmsStack.Encode(Msg);
        //__fline;
        //        std::cout << "[" <<  __CSmsStack.ToString() << "]" << std::endl;
        //        std::cout << __CSmsStack.ToString().size() << std::endl;
        //        std::cout << strEncoded.size() << std::endl;
        //        std::cout << strEncoded << std::endl;

        CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );
    }
    // decode after encode
    {
        CAckMsgSimple Msg;

        __CSmsStack.Decode(Msg);

        //        cout << endl;
        //        Msg.dump();
        //        cout << endl;

        CPPUNIT_ASSERT(Msg.result == result );
    }

}
void CTestMan::testEncodeCAckMsgSession()
{
    CSmsStack __CSmsStack;

    //std::cout<< endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    {
        CAckMsgSession Msg;
        Msg.result = "100";
        Msg.reason = "no such msg";
        Msg.SessionId = "76D663CC00754954434A4FBF37C4CC4E";
        //std::string strEncoded = "{\n   \"SessionId\" : \"76D663CC00754954434A4FBF37C4CC4E\",\n   \"reason\" : \"no such msg\",\n   \"result\" : \"100\"\n}\n";
        std::string strEncoded = "{ \"SessionId\" : \"76D663CC00754954434A4FBF37C4CC4E\", \"reason\" : \"no such msg\", \"result\" : \"100\" }\n";
        __CSmsStack.Encode(Msg);

        //        std::cout << __CSmsStack.ToString() << std::endl;
        //        std::cout << __CSmsStack.ToString().size() << std::endl;
        //        std::cout << strEncoded.size() << std::endl;
        //        std::cout << strEncoded << std::endl;
        CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );
    }

    // decode after encode
    {
        CAckMsgSession Msg;

        __CSmsStack.Decode(Msg);

        //        cout << endl;
        //        Msg.dump();
        //        cout << endl;

        CPPUNIT_ASSERT(Msg.result == "100" );
    }
}

void CTestMan::testED_CSmsMsg_RegisterReq							   ()
{
    CSmsStack __CSmsStack;

    //std::cout << endl << "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-" << std::endl;
    CSmsMsg_RegisterReq Msg;
    Msg.ProductID		= "VProductID";
    Msg.AuthCode 	= "VAuthCode";
    Msg.AuthName		= "VAuthName";
    Msg.Password	= "VPassword";

    Msg.m_LocalStatus.LocalIp = "VLocalIp";
    Msg.m_LocalStatus.LocalDateTime = "VLocalDateTime";

    //std::string strEncoded = "{\n   \"AuthCode\" : \"VAuthCode\",\n   \"AuthName\" : \"VAuthName\",\n   \"Password\" : \"VPassword\",\n   \"ProductID\" : \"VProductID\"\n}\n";
    std::string strEncoded = "{ \"AuthCode\" : \"VAuthCode\", \"AuthName\" : \"VAuthName\", \"LocalDateTime\" : \"VLocalDateTime\", \"LocalIp\" : \"VLocalIp\", \"Password\" : \"VPassword\", \"ProductID\" : \"VProductID\" }\n";

    __CSmsStack.Encode(Msg);

    //std::cout << __CSmsStack.ToString() << std::endl;
    //std::cout << __CSmsStack.ToString().size() << std::endl;
    //std::cout << strEncoded.size() << std::endl;
    //std::cout << strEncoded << std::endl;

    CPPUNIT_ASSERT(__CSmsStack.ToString() == strEncoded );
    /*
    {
       "AuthCode" : "VAuthCode",
       "AuthName" : "VAuthName",
       "LocalDateTime" : "VLocalDateTime",
       "LocalIp" : "VLocalIp",
       "Password" : "VPassword",
       "ProductID" : "VProductID"
    }
    */
    // decode after encode
    CSmsMsg_RegisterReq Msg2;
    __CSmsStack.Decode(Msg2);
    CPPUNIT_ASSERT(Msg.ProductID == "VProductID" );

    //Msg2.dump();



    /////////////////////////////////////////////////////////
    CSmsStack __CSmsStack_197;
    CSmsMsg_RegisterAck Msg_RegisterAck;
    CSmsMsg_ServiceAgent __service;

    //?y3?ê
    __service.ServiceName        = "PubServiceGetPubIP4";
    __service.ServiceLocation  = "http://pubservice.ezlibs.com:60089/PubService/getPubIP4";
    //Msg_RegisterAck.m_Service.push_back(__service);
    Msg_RegisterAck.m_Service.push_back(__service);
    Msg_RegisterAck.m_ackInfo.result = "0";
    Msg_RegisterAck.m_ackInfo.reason = "ok";
    Msg_RegisterAck.m_Session = "00000000000000000000";
    __CSmsStack_197.Encode(Msg_RegisterAck);
    //std::cout << std::endl;
    //std::cout << __CSmsStack_197.ToString() << std::endl;

    ///////////////////////////////////////////////////////////

    CSmsStack __CSmsStackAck;

    std::string strAck = "[{\"Service\":[{\"ServiceLocation\":\"http://pubservice.ezlibs.com:60089/PubService/getPubIP4\",\"ServiceName\":\"PubServiceGetPubIP4\"},{\"ServiceLocation\":\"http://pubservice.ezlibs.com:60089/PubService/getPubIP4\",\"ServiceName\":\"PubServiceGetPubIP4\"}],\"reason\":\"ok\",\"result\":\"0\",\"session\":\"00000000000000000000\"}]";
    //__CSmsStackAck.Parse(__CSmsStack_197.ToString());
    __CSmsStackAck.Parse(strAck);

    //    std::cout << std::endl << "__CSmsStackAck.Parse" <<std::endl;
    //    std::cout << __CSmsStackAck.ToString() << std::endl;

    CSmsMsg_RegisterAck __ackMsg;
    // strAck 前后多了[]，数据非法， so failed
    CPPUNIT_ASSERT(__CSmsStackAck.Decode(__ackMsg) == false );

    __CSmsStackAck.Parse(__CSmsStack_197.ToString());
    CPPUNIT_ASSERT(__CSmsStackAck.Decode(__ackMsg) == true );
    CPPUNIT_ASSERT(__ackMsg.m_Session == Msg_RegisterAck.m_Session );

    //    if (__CSmsStackAck.Decode(__ackMsg))
    //    {
    //        std::cout << std::endl << "__ackMsg.dump();" << std::endl << "-----------------------" << std::endl;
    //        __ackMsg.dump();
    //        std::cout << std::endl << "-----------------------" << std::endl;
    //    }
    //    else
    //    {
    //        std::cout << std::endl << "__CSmsStackAck.Decode(__ackMsg) FAILED!!" << std::endl;
    //    }

}

