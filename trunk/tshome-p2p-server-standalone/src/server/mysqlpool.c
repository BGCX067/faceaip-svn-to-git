/*
 * mysqlpool.c
 *
 *  Created on: 2012-2-18
 *      Author: sunzq
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"
#include "../Logs.h"
#include "EZThread.h"
#include "EZTimer.h"
#include "EZSignals.h"
#include <vector>
#include <iostream>
#include "../CommonInclude.h"

#define DB_POOL_SIZE 10

#define THIS_FILE "mysqlpool.c"
std::vector<MysqlConn> g_db_pool;
static int dbpool_inited = TS_FALSE;				//init flag.
//static char *method;				//method name for log.
//static char *msg;				//message of log.
static CEZMutex m_mutexCommunicationStatus;


int pool_init_full(const char *host,
                   const char *user,
                   const char *passwd,
                   const char *db,
                   unsigned int port,
                   const char *unix_socket,
                   unsigned long clientflag)
{
    pj_status_t status;
    if(TS_TRUE == dbpool_inited)
    {
        //method = "pool_init_full";
        //msg = "the g_db_pool had been initialized";
        //PJ_LOG(3,(THIS_FILE, "(%s):%s", method, msg));

        LOG4CPLUS_ERROR(LOG_DB, "the g_db_pool had been initialized:");
        status = TS_FAILED;
        return status;
    }
    pj_assert(NULL != db);
    pj_assert(NULL != user);
    pj_assert(NULL != host);
    int i;
    for(i = 0; i < DB_POOL_SIZE; i++)
    {
        MysqlConn mysqlConn;
        mysqlConn.mysql = mysql_init(NULL);
        assert(mysqlConn.mysql);
        int reconnect = 1;
        mysql_options(mysqlConn.mysql, MYSQL_SET_CHARSET_NAME, "utf8");
        mysql_options(mysqlConn.mysql, MYSQL_OPT_RECONNECT, &reconnect);
        MYSQL *pMysql=mysql_real_connect(mysqlConn.mysql, host, user, passwd, db, port, unix_socket, clientflag);
        if (pMysql)
        {
            g_db_pool.push_back(mysqlConn);

            //printf("mysql_real_connect %s, %s, %s, %s, %d succeeded.\n", host, user, passwd, db, port);
            LOG4CPLUS_DEBUG(LOG_DB, "mysql_real_connect:"
                           << host<<" "<<user<<" "<<passwd<<" "<<db<<" "<<port<<" succeeded");
        }
        else
        {
            //printf("mysql_real_connect %s, %s, %s, %s, %d failed.\n", host, user, passwd, db, port);
            LOG4CPLUS_ERROR(LOG_DB, "mysql_real_connect:"
                            << host<<" "<<user<<" "<<passwd<<" "<<db<<" "<<port<<" failed,exit ...");
            /*pool_init_full(host,
            			   user,
            			   passwd,
            			   db,
            			   port,
            			   unix_socket,
            			   clientflag);*/
            //__trip;
            std:: cout << "mysql_real_connect:"
                            << host<<" "<<user<<" "<<passwd<<" "<<db<<" "<<port<<" failed,exit ..." << std::endl;

            exit(-1);
        }
    }
    dbpool_inited = TS_TRUE;
    return TS_SUCCESS;
}

#if 0
PJ_DEF(pj_status_t)  pool_init()
{
    return pool_init_full(NULL, NULL, NULL, NULL, 0, NULL, 0);
}
#endif

MysqlConn pool_getConn()
{
    pj_assert(dbpool_inited);
    MysqlConn conn;
    CEZGuard guard(m_mutexCommunicationStatus);
    if(pool_getSize() > 0)
    {
        conn = g_db_pool[0];
        g_db_pool.erase(g_db_pool.begin());
    }
	else
	{
		conn.mysql = NULL;
		LOG4CPLUS_INFO(LOG_DB, "MYSQL pool is busy!");
	}
    return conn;
}
int pool_getSize()
{
    pj_assert(dbpool_inited);
    return g_db_pool.size();
}
void pool_recycle(const MysqlConn &conn)
{
    pj_assert(dbpool_inited);
    CEZGuard guard(m_mutexCommunicationStatus);
    g_db_pool.push_back(conn);
    //printf("g_db_pool.size():%d\n", g_db_pool.size());
}
int pool_isEmpty()
{
    pj_assert(dbpool_inited);
    return pool_getSize() == 0;
}
void pool_close()
{
    pj_assert(dbpool_inited);

    CEZGuard guard(m_mutexCommunicationStatus);
    if(pool_getSize() == DB_POOL_SIZE)
    {
        int i;
        for (i=0; i<DB_POOL_SIZE; ++i)
        {
            MysqlConn conn = g_db_pool[i];
            if (conn.mysql != NULL)
            {
                mysql_close(conn.mysql);
            }
        }
        g_db_pool.clear();

        dbpool_inited = TS_FALSE;
    }
}
