#ifndef __FILTERS_PARAMETERSFACTORY_IPARAMETERSFACTORY_H__
#define __FILTERS_PARAMETERSFACTORY_IPARAMETERSFACTORY_H__

#include "../FilterParameter.h"

class IParametersFactory
{
public:
    IParametersFactory( void ) { }
    virtual ~IParametersFactory( void ) { }

    virtual CFilterParameterMap Create( void ) const = 0;
};

#endif // __FILTERS_PARAMETERSFACTORY_IPARAMETERSFACTORY_H__
