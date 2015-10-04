#pragma once

#include <QWidget>
#include <QComboBox>
#include <QLayout>

class DeviceWidget : public QWidget
{
	Q_OBJECT
public:
	explicit DeviceWidget(QWidget *parent = 0);
	virtual ~DeviceWidget();

private slots:
	void OnDeviceListActivated(int index);

private:
	QComboBox *_device_list;
	QVBoxLayout *_vertical_layout;
};
