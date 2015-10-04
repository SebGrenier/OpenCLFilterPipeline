#include "NoFilter.h"

void NoFilter::Apply(const Image &input, Image &output)
{
	output = input;
}

void NoFilter::BuildParameterList()
{}
