/**
 **	\file FileStream.h
**/

#ifndef _SOCKETS_FileStream_H
#define _SOCKETS_FileStream_H

#include "IStream.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class IFile;

class FileStream : public IStream
{
public:
	FileStream(IFile& file);

	size_t IStreamRead(char *buf, size_t max_sz);

	void IStreamWrite(const char *buf, size_t sz);

private:
	IFile& m_file;
};

#ifdef SOCKETS_NAMESPACE
}
#endif

#endif // _SOCKETS_FileStream_H
