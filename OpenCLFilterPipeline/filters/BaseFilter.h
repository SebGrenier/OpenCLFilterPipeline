#pragma once
#include "FilterParameter.h"
#include "../Image.h"
#include <string>

class BaseFilter
{
public:
	BaseFilter();
	virtual ~BaseFilter();

	virtual void Apply(const Image &input, Image &output) = 0;
	virtual void BuildParameterList() = 0;
	virtual std::string Name() const = 0;

	CFilterParameterMap* Parameters() { return _parameters; }

protected:
	CFilterParameterMap *_parameters;
};