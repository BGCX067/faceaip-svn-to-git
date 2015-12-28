/**
 **	\file HttpRequest.h
 **	\date  2007-10-05
**/
#ifndef _SOCKETS_HttpRequest_H
#define _SOCKETS_HttpRequest_H

#include "HttpTransaction.h"
#include "HttpdCookies.h"
#include "IFile.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class HttpdForm;
class IFile;

class HttpRequest : public HttpTransaction
{
public:
	HttpRequest();
	/** Create from plain old cgi request */
#ifndef _WIN32
	HttpRequest(FILE *);
#endif
	HttpRequest(const HttpRequest& src);
	~HttpRequest();

	HttpRequest& operator=(const HttpRequest& src);

	/** Get, Post */
	void SetHttpMethod(const std::string& value);
	const std::string& HttpMethod() const;

	/** HTTP/1.x */
	void SetHttpVersion(const std::string& value);
	const std::string& HttpVersion() const;

	void SetUri(const std::string& value);
	const std::string& Uri() const;

	void SetRemoteAddr(const std::string& value);
	const std::string& RemoteAddr() const;

	void SetRemoteHost(const std::string& value);
	const std::string& RemoteHost() const;

	void SetServerName(const std::string& value);
	const std::string& ServerName() const;

	void SetServerPort(int value);
	int ServerPort() const;

	void SetIsSsl(bool value);
	bool IsSsl() const;

	/** Set / Read attribute value */
	void SetAttribute(const std::string& key, const std::string& value);
	void SetAttribute(const std::string& key, long value);
	const std::string& Attribute(const std::string& key) const;

	const Utility::ncmap<std::string>& Attributes() const;

	/** Cookies */
	void AddCookie(const std::string& );
	const Utility::ncmap<std::string>& CookieMap() const { return m_cookie; }

	/** Open file for body data */
	void InitBody( size_t sz );

	/** Write body data */
	void Write( const char *buf, size_t sz );

	/** No more writing */
	void CloseBody();

	void ParseBody();

	const HttpdForm& Form() const;
	const HttpdCookies& Cookies() const;

	const IFile *BodyFile() const { return m_body_file.get(); }

	void Reset();

private:
	std::string m_method;
	std::string m_protocol;
	std::string m_req_uri;
	std::string m_remote_addr;
	std::string m_remote_host;
	std::string m_server_name;
	int m_server_port;
	bool m_is_ssl;
	Utility::ncmap<std::string> m_attribute;
	std::string m_null;
	mutable std::auto_ptr<IFile> m_body_file;
	mutable std::auto_ptr<HttpdForm> m_form;
	HttpdCookies m_cookies;
	Utility::ncmap<std::string> m_cookie;

}; // end of class


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _SOCKETS_HttpRequest_H

