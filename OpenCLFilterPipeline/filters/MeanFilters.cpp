#include "MeanFilters.h"

MeanFilter_CPU::MeanFilter_CPU()
	: BaseFilter()
{
}

MeanFilter_CPU::~MeanFilter_CPU()
{
}

void MeanFilter_CPU::Apply(const Image &input, Image &output)
{
	output = input;
}

void MeanFilter_CPU::BuildParameterList()
{
	_parameters->AddParameter(new CFilterInt(1, "Radius", "Radius of the kernel", 1, 9999));
}

