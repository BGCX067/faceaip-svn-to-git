/**
 **	\file XmlException.h
 **	\date  2008-02-09
**/

#ifndef _XmlException_H
#define _XmlException_H

#include "sockets-config.h"
#ifdef ENABLE_XML
#include "Exception.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


class XmlException : public Exception
{
public:
	XmlException(const std::string& descr);

	XmlException(const XmlException& x) : Exception(x) {} // copy constructor

private:
	XmlException& operator=(const XmlException& ) { return *this; } // assignment operator

};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // ENABLE_XML
#endif // _XmlException_H
