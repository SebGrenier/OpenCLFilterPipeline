#ifndef __FILTERS_PARAMETERSFACTORY_EMPTYPARAMETERSFACTORY_H__
#define __FILTERS_PARAMETERSFACTORY_EMPTYPARAMETERSFACTORY_H__

#include "IParametersFactory.h"

class CEmptyParametersFactory : public IParametersFactory
{
public:
    CEmptyParametersFactory( void ) { }
    virtual ~CEmptyParametersFactory( void ) { }

    virtual CFilterParameterMap Create( void ) const
    {
        return CFilterParameterMap( );
    }
};

#endif // __FILTERS_PARAMETERSFACTORY_EMPTYPARAMETERSFACTORY_H__
