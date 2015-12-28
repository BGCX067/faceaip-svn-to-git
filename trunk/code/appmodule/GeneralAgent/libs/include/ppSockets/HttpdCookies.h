/** \file HttpdCookies.h
*/

#ifndef _SOCKETS_HttpdCookies_H
#define _SOCKETS_HttpdCookies_H

#include "sockets-config.h"
#include <list>
#include <string>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


//! Store the cookies name/value pairs.



//! Retrieve and manage cookies during a cgi call.
class HTTPSocket;

/** HTTP Cookie parse/container class. 
\sa HttpdSocket
\sa HttpdForm 
\ingroup webserver */
class HttpdCookies 
{
	/** list of key/value structs. */
	typedef std::list<std::pair<std::string, std::string> > cookie_v;

public:
	HttpdCookies();
	HttpdCookies(const std::string& query_string);
	~HttpdCookies();

	void add(const std::string& s);

	bool getvalue(const std::string&,std::string&) const;
	void replacevalue(const std::string& ,const std::string& );
	void replacevalue(const std::string& ,long);
	void replacevalue(const std::string& ,int);
	size_t getlength(const std::string& ) const;
	void setcookie(HTTPSocket *,const std::string& d,const std::string& p,const std::string& c,const std::string& v);
	void setcookie(HTTPSocket *,const std::string& d,const std::string& p,const std::string& c,long v);
	void setcookie(HTTPSocket *,const std::string& d,const std::string& p,const std::string& c,int v);
	const std::string& expiredatetime() const;

	cookie_v& GetHttpdCookies() { return m_cookies; }

	void Reset();

private:
	cookie_v m_cookies;
	mutable std::string m_date;
};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_HttpdCookies_H

