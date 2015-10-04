#pragma once
#include "FilterParameter.h"

class BaseFilter
{
public:
	BaseFilter();
	virtual ~BaseFilter();

	virtual void Apply(unsigned char image_data, int image_width, int image_height, int image_depth) = 0;
	virtual void BuildParameterList() = 0;

private:
	CFilterParameterMap *_parameters;
};