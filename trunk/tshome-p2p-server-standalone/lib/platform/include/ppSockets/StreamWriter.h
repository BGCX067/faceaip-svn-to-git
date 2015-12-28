/**
 **	\file StreamWriter.h
 **	\date  2008-12-20
**/

#ifndef _STREAMWRITER_H
#define _STREAMWRITER_H

#include "sockets-config.h"
#include <string>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class IStream;

class StreamWriter
{
public:
	StreamWriter(IStream& stream);
	virtual ~StreamWriter() {}

	StreamWriter& operator<<(const char *);
	StreamWriter& operator<<(const std::string&);
	StreamWriter& operator<<(short);
	StreamWriter& operator<<(int);
	StreamWriter& operator<<(long);
	StreamWriter& operator<<(double);

private:
	IStream& m_stream;

};

#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _STREAMWRITER_H
