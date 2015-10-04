#pragma once

#include "BaseFilter.h"

class NoFilter : public BaseFilter
{
public:
	NoFilter() : BaseFilter() {}
	virtual ~NoFilter() {}

	virtual void Apply(const Image &input, Image &output) override;
	virtual void BuildParameterList() override;
	virtual std::string Name() const override { return "Dummy Filter"; }

};