#include "FiltersHolder.h"
#include "Filters/NoFilter.h"
#include "Filters/MeanFilters.h"
#include <assert.h>

FiltersHolder* FiltersHolder::_instance = 0;

FiltersHolder::FiltersHolder()
	: _current_filter_index(0)
{
}

FiltersHolder::~FiltersHolder()
{
	auto it = _filters.begin();
	auto end = _filters.end();
	for (; it != end; ++it)
	{
		delete (*it);
	}
	_filters.clear();
}

void FiltersHolder::InitFilters()
{
	_filters.push_back(new NoFilter());
	_filters.push_back(new MeanFilter_CPU());

	_current_filter_index = 0;
}

BaseFilter* FiltersHolder::CurrentFilter()
{
	assert(_current_filter_index >= 0 && _current_filter_index < _filters.size());
	return _filters[_current_filter_index];
}

void FiltersHolder::SetCurrentFilterIndex(int index)
{
	assert(index >= 0 && index < _filters.size());
	_current_filter_index = index;
}

FiltersHolder* FiltersHolder::Instance()
{
	if (!_instance)
	{
		_instance = new FiltersHolder();
	}
	return _instance;
}
