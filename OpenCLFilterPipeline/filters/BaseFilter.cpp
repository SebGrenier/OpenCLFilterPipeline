#include "BaseFilter.h"

BaseFilter::BaseFilter()
	: _parameters(0)
{
	_parameters = new CFilterParameterMap();
}

BaseFilter::~BaseFilter()
{
	if (_parameters)
	{
		delete _parameters;
	}
}

