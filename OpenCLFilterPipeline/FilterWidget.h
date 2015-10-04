#pragma once

#include "Filters/FilterPanel.h"
#include <QWidget>
#include <QComboBox>
#include <QLayout>

class FilterWidget : public QWidget
{
	Q_OBJECT
public:
	explicit FilterWidget(QWidget *parent = 0);
	~FilterWidget();

private slots:
	void OnFilterListActivated(int index);

private:
	QVBoxLayout *_vertical_layout;
	QComboBox *_filter_list;
	CFilterPanel *_filter_panel;
};