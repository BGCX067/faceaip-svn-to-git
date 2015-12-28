#ifndef _XmlDocument_H
#define _XmlDocument_H

#include "sockets-config.h"

#ifdef ENABLE_XML

#include <string>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif
namespace Xml {


class XmlDocument
{
public:
	XmlDocument(const std::string& filename,const std::string& verify_ns = "",const std::string& verify_root = "");
	virtual ~XmlDocument();

	/** Document parse successful. */
	bool IsOk() { return m_ok; }

	operator xmlDocPtr();

protected:
	XmlDocument(const XmlDocument& ) {} // copy constructor

private:
	XmlDocument& operator=(const XmlDocument& ) { return *this; } // assignment operator

	xmlDocPtr m_doc;
	bool m_ok;
};


}
#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // ENABLE_XML

#endif // _XmlDocument_H
