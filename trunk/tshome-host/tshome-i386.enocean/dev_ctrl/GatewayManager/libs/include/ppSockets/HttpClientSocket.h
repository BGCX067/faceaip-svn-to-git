/**
 **	\file HttpClientSocket.h
 **	\date  2006-04-20
**/
#ifndef _SOCKETS_HttpClientSocket_H
#define _SOCKETS_HttpClientSocket_H

#include "sockets-config.h"
#include "HTTPSocket.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class IFile;

/** Get http response to file or memory. 
	\ingroup http */
class HttpClientSocket : public HTTPSocket
{
public:
	HttpClientSocket(ISocketHandler&);
	HttpClientSocket(ISocketHandler&,const std::string& url_in);
	HttpClientSocket(ISocketHandler&,const std::string& host, port_t port, const std::string& url_in);
	~HttpClientSocket();

	/** Parse url to protocol,host,port,url and file. */
	void Url(const std::string& url_in,std::string& host,port_t& port);

	void OnFirst();
	void OnHeader(const std::string&,const std::string&);
	void OnHeaderComplete();
	void OnData(const char *,size_t);
	void OnDelete();

	/** New callback method fires when all data is received. */
	virtual void OnContent();

	/** Write response to this file */
	void SetFilename(const std::string& );
	const std::string& Filename() const { return m_filename; }

	/** Store response in this buffer. */
	void SetDataPtr(unsigned char *,size_t);

	/** Get response headers. */
	const std::string& GetContent();

	/** Get size of response body. */
	size_t GetContentLength();

	/** Get content type from response header. */
	const std::string& GetContentType();

	/** Get size of received response body. */
	size_t GetContentPtr();
	/** Get size of received response body. */
	size_t GetPos();

	/** Complete response has been received. */
	bool Complete();

	/** Get ptr to response data buffer. */
	const unsigned char *GetDataPtr() const;

	/** Close socket when response received. */
	void SetCloseOnComplete(bool = true);

	/** Get protocol used from url. */
	const std::string& GetUrlProtocol();
	/** Get hostname from url. */
	const std::string& GetUrlHost();
	/** Get port from url. */
	port_t GetUrlPort();
	/** Get filename part of url. */
	const std::string& GetUrlFilename();

protected:
	HttpClientSocket(const HttpClientSocket& s) : HTTPSocket(s) {} // copy constructor
	HttpClientSocket& operator=(const HttpClientSocket& ) { return *this; } // assignment operator
private:
	std::string m_filename; ///< Filename to write response to
	unsigned char *m_data_ptr; ///< Ptr to buffer where to store response
	size_t m_data_size; ///< Max size of data buffer
	size_t m_content_length; ///< Content-length header received from remote
	std::string m_content; ///< Received http headers
	bool m_data_ptr_set; ///< Buffer set from outside, do not delete
	IFile *m_fil; ///< Output file
	size_t m_content_ptr; ///< Number of bytes received from body
	bool m_b_complete; ///< The entire content-length number of bytes has been received
	bool m_b_close_when_complete; ///< Close when the full response has been received
	std::string m_protocol; ///< Protocol part of url_in
	std::string m_host; ///< Hostname from url_in
	port_t m_port; ///< Port from url_in
	std::string m_url_filename; ///< Filename from url_in
	std::string m_content_type; ///< Content-type: header from response
};




#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
#endif // _SOCKETS_HttpClientSocket_H

