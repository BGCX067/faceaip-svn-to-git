/*
 * mysqlpool.h
 *
 *  Created on: 2012-2-18
 *      Author: sunzq
 */

#ifndef MYSQLPOOL_H_
#define MYSQLPOOL_H_
#include <pjlib.h>
#include <mysql/mysql.h>

#ifdef __cplusplus
extern "C" {
#endif

/*mysql connection pool*/
typedef struct mysql_conn{
	PJ_DECL_LIST_MEMBER(struct mysql_conn);	/**< Generic list member.   */
	MYSQL *mysql;								//real mysql connection 
}MysqlConn;

/*
 *init pool
 *@param size 					pool size
*/
//PJ_DEF(pj_status_t)  pool_init();

/*
 *init pool with full parameters.
 *@param size					pool size
 *@param host					mysql database host address
 *@param user					database userName
 *@param passwd					database password
 *@param db					database name
 *@param port					port
 *@param unix_socket				
 *@param clientflag				
*/
int pool_init_full(const char *host,
				   const char *user,
				   const char *passwd,
				   const char *db,
				   unsigned int port,
				   const char *unix_socket,
				   unsigned long clientflag);

/*
 *get a mysql connection, and remove it from the pool.
 *@return MYSQL * 				the mysql connection.
*/
MysqlConn pool_getConn();

/*
 *get pool size.
*/
int pool_getSize();

/*
 *recycle a mysql connection to the pool.
*/
void pool_recycle(const MysqlConn &conn);

/*
 *check the pool whether empty.
*/
int pool_isEmpty();

/*
 *close the pool, and release all .
*/
void pool_close();

#ifdef __cplusplus
}
#endif


#endif /* MYSQLPOOL_H_ */
