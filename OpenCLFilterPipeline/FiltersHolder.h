#pragma once
#include "Filters/BaseFilter.h"
#include <vector>

class FiltersHolder
{
public:
	static FiltersHolder* Instance();

	void InitFilters();

	BaseFilter* CurrentFilter();
	int CurrentFilterIndex() const { return _current_filter_index; }
	void SetCurrentFilterIndex(int index);
	const std::vector<BaseFilter*>& Filters() const { return _filters; }

private:
	FiltersHolder();
	~FiltersHolder();

	static FiltersHolder *_instance;

	std::vector<BaseFilter*> _filters;
	int _current_filter_index;
};