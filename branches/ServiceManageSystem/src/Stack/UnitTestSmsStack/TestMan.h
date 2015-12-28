/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * TestMan.h - _explain_
 *
 * Copyright (C) 2012 tiansu-china.com, All Rights Reserved.
 *
 * $Id: TestMan.h 0001 2012-5-8 14:38:29Z wu_junjie $
 *
 *  Explain:
 *     TeHomeMsg -- Telecom eHome Msg
 *
 *  Update:
 *     2012-5-8 14:38:33 WuJunjie 549 Create
 *
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

#if !defined(_MANTEST_H_)
#define _MANTEST_H_

#include <cppunit/extensions/HelperMacros.h>

class CTestMan : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( CTestMan );
    CPPUNIT_TEST( testSmsStackParse );

    CPPUNIT_TEST( testEncodeCAckMsgSimple );
    CPPUNIT_TEST( testEncodeCAckMsgSession );
    CPPUNIT_TEST( testED_CSmsMsg_RegisterReq                         );

    CPPUNIT_TEST_SUITE_END();

public:
    void testSmsStackParse();
    void testEncodeCAckMsgSimple();
    void testEncodeCAckMsgSession();

    void testED_CSmsMsg_RegisterReq                        ();


    void setUp();
    void tearDown();
};

#endif // _MANTEST_H_
