/** \file IModel.h
 **	\date  2011-07-17
**/

#ifndef _SOCKETS_IModel_H
#define _SOCKETS_IModel_H

#include "sockets-config.h"

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

class Json;

/** IModel interface.
	\ingroup tools */
class IModel
{
public:
	virtual ~IModel() {}

	virtual void IModel_ToJson(Json& json) const = 0;
	virtual void IModel_FromJson(const Json& json) = 0;
};


#ifdef SOCKETS_NAMESPACE
}
#endif
#endif // _SOCKETS_IModel_H

