#include "OpenCLUtils.h"
#include <iostream>
#include <assert.h>

OpenCLUtils * OpenCLUtils::_instance = 0;

OpenCLDevice::OpenCLDevice(cl::Device device, cl::CommandQueue queue, cl::Context context)
	: _device(device)
	, _command_queue(queue)
	, _context(context)
{
	_name = _device.getInfo<CL_DEVICE_NAME>();
}

OpenCLDevice::~OpenCLDevice()
{
}

OpenCLUtils* OpenCLUtils::Instance()
{
	if (_instance == 0)
	{
		_instance = new OpenCLUtils();
	}
	return _instance;
}

bool OpenCLUtils::CheckOpenCLError(cl_int returnCode, const std::string &ErrorString)
{
	if (returnCode != CL_SUCCESS)
	{
		std::cout << ErrorString << std::endl;
		std::cout << "Error code : " << returnCode << std::endl;
		return false;
	}
	return true;
}

OpenCLUtils::OpenCLUtils()
	: _is_initialized(false)
{
}

OpenCLUtils::~OpenCLUtils()
{
}

bool OpenCLUtils::InitOpenCL()
{
	if (_is_initialized)
		return true;

	// Get platforms and devices information
	cl::Platform::get(&_platforms);

	for (auto i = 0; i < _platforms.size(); ++i)
	{
		std::vector<cl::Device> platofmr_devices;
		_platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &platofmr_devices);

		cl::Context context = cl::Context(platofmr_devices);
		std::vector<cl::Device> ctx_devices = context.getInfo<CL_CONTEXT_DEVICES>();

		for (auto j = 0; j < ctx_devices.size(); ++j)
		{
			cl::Device device = ctx_devices[j];
			cl::CommandQueue cq(context, device);

			OpenCLDevice dev(device, cq, context);
			_devices.push_back(dev);
		}
	}

	if (_devices.size() > 0)
	{
		_current_device_index = 0;
		_is_initialized = true;
	}

	return _is_initialized;
}

OpenCLDevice& OpenCLUtils::GetCurrentDevice()
{
	assert(_current_device_index >= 0 && _current_device_index < _devices.size());

	return _devices[_current_device_index];
}

void OpenCLUtils::SetCurrentDeviceIndex(int index)
{
	assert(index >= 0 && index < _devices.size());
	_current_device_index = index;
}