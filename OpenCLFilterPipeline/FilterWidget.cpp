#include "FilterWidget.h"
#include "FiltersHolder.h"

FilterWidget::FilterWidget(QWidget* parent)
	: QWidget(parent)
{
	_vertical_layout = new QVBoxLayout(this);
	_filter_list = new QComboBox();
	_filter_list->setToolTip("Select Filter");
	_vertical_layout->addWidget(_filter_list);

	// Populate device list
	auto filters = FiltersHolder::Instance()->Filters();
	for (auto i = 0; i < filters.size(); ++i)
	{
		_filter_list->addItem(QString::fromStdString(filters[i]->Name()));
	}
	_filter_list->setCurrentIndex(FiltersHolder::Instance()->CurrentFilterIndex());

	// Connect signal to slot
	connect(_filter_list, SIGNAL(activated()), this, SLOT(OnDeviceListActivated()));
}

FilterWidget::~FilterWidget()
{
	if (_vertical_layout)
	{
		// This will delete all its children
		delete _vertical_layout;
	}
}


void FilterWidget::OnFilterListActivated(int index)
{
	FiltersHolder::Instance()->SetCurrentFilterIndex(index);
}