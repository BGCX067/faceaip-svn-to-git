/**
 **	\file Exception.h
 **	\date  2007-09-28
**/
#ifndef _Sockets_Exception_H
#define _Sockets_Exception_H

#include "sockets-config.h"
#include <string>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif



class Exception
{
public:
	Exception(const std::string& description);
	virtual ~Exception() {}

	virtual const std::string ToString() const;
	virtual const std::string Stack() const;

	Exception(const Exception& ) {} // copy constructor

	Exception& operator=(const Exception& ) { return *this; } // assignment operator

private:
	std::string m_description;
	std::string m_stack;

};



#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif

#endif // _Sockets_Exception_H

