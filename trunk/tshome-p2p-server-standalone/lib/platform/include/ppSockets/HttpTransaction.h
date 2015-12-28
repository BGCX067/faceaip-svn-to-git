/**
 **	\file HttpTransaction.h
 **	\date  2007-10-05
**/
#ifndef _SOCKETS_HttpTransaction_H
#define _SOCKETS_HttpTransaction_H

#include "Utility.h"


#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class HttpTransaction
{
public:
	HttpTransaction();
	virtual ~HttpTransaction();

	/** Set / Read http header value */
	void SetHeader(const std::string& key, const std::string& value);
	void SetHeader(const std::string& key, long value);
	const std::string& Header(const std::string& key) const;

	void SetAccept(const std::string& value);
	const std::string& Accept() const;

	void SetAcceptCharset(const std::string& value);
	const std::string& AcceptCharset() const;

	void SetAcceptEncoding(const std::string& value);
	const std::string& AcceptEncoding() const;

	void SetAcceptLanguage(const std::string& value);
	const std::string& AcceptLanguage() const;

	void SetConnection(const std::string& value);
	const std::string& Connection() const;

	void SetContentType(const std::string& value);
	const std::string& ContentType() const;

	void SetContentLength(long value);
	long ContentLength() const;

	void SetHost(const std::string& value);
	const std::string& Host() const;
	const std::string HostOnly() const;

	void SetPragma(const std::string& value);
	const std::string& Pragma() const;

	void SetReferer(const std::string& value);
	const std::string& Referer() const;

	void SetUserAgent(const std::string& value);
	const std::string& UserAgent() const;

	const Utility::ncmap<std::string>& Headers() const;

	virtual void Reset();

private:
	Utility::ncmap<std::string> m_header;
	std::string m_null;

}; // end of class


#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _SOCKETS_HttpTransaction_H

