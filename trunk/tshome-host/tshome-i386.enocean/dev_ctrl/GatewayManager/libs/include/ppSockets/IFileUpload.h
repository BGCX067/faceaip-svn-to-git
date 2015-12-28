/** \file IFileUpload.h
 **	\date  2009-04-22
**/

#ifndef _SOCKETS_IFileUpload_H
#define _SOCKETS_IFileUpload_H

#include "sockets-config.h"
#include <string>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class IStream;

/** Multipart form file upload callback.
	\ingroup webserver */
class IFileUpload
{
public:
	virtual ~IFileUpload() {}

	virtual IStream& IFileUploadBegin(const std::string& input_name, const std::string& filename, const std::string& content_type) = 0;

	virtual void IFileUploadEnd() = 0;
};


#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_IFileUpload_H

