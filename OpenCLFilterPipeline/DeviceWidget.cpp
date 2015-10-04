#include "DeviceWidget.h"
#include "OpenCLUtils.h"

DeviceWidget::DeviceWidget(QWidget* parent)
	: QWidget(parent)
	, _device_list(0)
	, _vertical_layout(0)
{
	_vertical_layout = new QVBoxLayout(this);
	_device_list = new QComboBox();
	_device_list->setToolTip("Select OpenCL Device");
	_vertical_layout->addWidget(_device_list);

	// Populate device list
	auto devices = OpenCLUtils::Instance()->Devices();
	for (auto i = 0; i < devices.size(); ++i)
	{
		_device_list->addItem(QString::fromStdString(devices[i].Name()));
	}
	_device_list->setCurrentIndex(OpenCLUtils::Instance()->GetCurrentDeviceIndex());

	// Connect signal to slot
	connect(_device_list, SIGNAL(activated()), this, SLOT(OnDeviceListActivated()));
}

DeviceWidget::~DeviceWidget()
{
	if (_vertical_layout)
	{
		// This will delete all its children
		delete _vertical_layout;
	}
}

void DeviceWidget::OnDeviceListActivated(int index)
{
	OpenCLUtils::Instance()->SetCurrentDeviceIndex(index);
}