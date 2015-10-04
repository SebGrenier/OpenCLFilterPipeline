#pragma once

#include "BaseFilter.h"

class MeanFilter_CPU : public BaseFilter
{
public:
	MeanFilter_CPU();
	~MeanFilter_CPU();

	virtual void Apply(const Image &input, Image &output) override;
	virtual void BuildParameterList() override;
	virtual std::string Name() const override { return "Mean Filter CPU"; }

private:
};