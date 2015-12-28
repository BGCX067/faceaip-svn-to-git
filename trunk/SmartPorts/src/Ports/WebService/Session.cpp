/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
/*
 * Session.cpp - _explain_
 *
 * Copyright (C) 2011 ezlibs.com, All Rights Reserved.
 *
 * $Id: Session.cpp 5884 2013-02-26 09:39:20Z WuJunjie $
 *
 *  Explain:
 *     -explain-
 *
 *  Update:
 *     2013-02-26 09:39:20  Create
 */
/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
#include "CommonInclude.h"
#include "Session.h"

#include "../Logs.h"

CSession::CSession()
{
    m_strUName="";

    m_iTimeout = SESSION_TIMEOUT_DEFAULT;
    m_ttLastRequestTime = time(NULL);
}

CSession::~CSession()
{
    ;
}
/* = */
CSession& CSession::operator = (const CSession& obj)
{
    this->m_strSession = obj.m_strSession;
    this->m_strUName = obj.m_strUName;
    this->m_ttLastRequestTime = obj.m_ttLastRequestTime;

    this->m_vAccess = obj.m_vAccess;

    return *this;
}

/* == */
bool CSession::operator==(const CSession& obj) const
{
    if ( this->m_strSession != obj.m_strSession
         || this->m_strUName != obj.m_strUName
         || this->m_ttLastRequestTime != obj.m_ttLastRequestTime
       )
        return false;

    return true;
}

/* != */
bool CSession::operator!=(const CSession& obj) const
{
    return !(*this==obj);
}

/* printf info when debugging */
void CSession::Dump()
{
    __fline;
    std::cout <<"CSession::Dump()" << std::endl;

    std::cout <<"m_strSession :"<<m_strSession << std::endl;
    std::cout <<"m_strUName :"<<m_strUName << std::endl;
    std::cout <<"m_ttLastRequestTime :"<<m_ttLastRequestTime << std::endl;
}

/*-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/

CSessionManager::CSessionManager()
{
    ;
}
CSessionManager::~CSessionManager()
{
    ;
}
// ÐÂsession£¬
int CSessionManager::newSession(const CSession &csSession)
{
    m_mapSession[csSession.m_strSession] = csSession;

    Maintain();

    LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "newSession:" << csSession.m_strSession);

    return 0;
}
int CSessionManager::deleteSession(std::string strSession)
{
    return 0;
}

int CSessionManager::findSession(std::string strSession, CSession &newSession)
{
    std::map<std::string , CSession>::iterator iter;
    iter = m_mapSession.find(strSession);

    if(iter != m_mapSession.end())
    {
        m_mutexmapSession.Lock();

        iter->second.m_ttLastRequestTime=time(NULL);
        newSession = iter->second;
		
        m_mutexmapSession.Unlock();

        return 0;
    }
    else
    {
        return -1;
    }

    return 0;
}

int CSessionManager::Maintain()
{
    std::map<std::string , CSession>::iterator iter;

    CEZLock __guard(m_mutexmapSession);

    for(iter = m_mapSession.begin(); iter != m_mapSession.end(); iter++)
    {
        if ((time(NULL)-iter->second.m_iTimeout) > iter->second.m_ttLastRequestTime)
        {
            LOG4CPLUS_DEBUG(LOG_WEBSERVICES, "Maintain erase:" << iter->second.m_strSession);

            m_mapSession.erase(iter);
        }
    }

    return 0;
}

