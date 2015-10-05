#include "FilterWidget.h"
#include "FiltersHolder.h"

FilterWidget::FilterWidget(QWidget* parent)
	: QWidget(parent)
	, _filter_panel(0)
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
	connect(_filter_list, SIGNAL(activated(int)), this, SLOT(OnFilterListActivated(int)));

	CreateFilterPanel();
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

	CreateFilterPanel();
}

void FilterWidget::CreateFilterPanel()
{
	if (_filter_panel)
	{
		_vertical_layout->removeWidget(_filter_panel);
		delete _filter_panel;
	}

	auto filter = FiltersHolder::Instance()->CurrentFilter();
	_filter_panel = new CFilterPanel(0, filter->Parameters(), QString::fromStdString(filter->Name()));
	_vertical_layout->addWidget(_filter_panel);
}