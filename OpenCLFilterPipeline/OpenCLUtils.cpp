#include "OpenCLUtils.h"
#include <iostream>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <algorithm>

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

cl::Program OpenCLUtils::GetProgram(cl::Context &context, const std::string& file_path)
{
	std::ifstream file(file_path.c_str());
	// Convert the program to a string
	std::ostringstream os;
	os << file.rdbuf();
	std::string sourceStr = os.str();


	// Create a source for the program to build
	auto sources = cl::Program::Sources(1, std::make_pair(sourceStr.c_str(), sourceStr.length() + 1));

	auto error = 0;
	auto program = cl::Program(context, sources, &error);

	if (file.is_open())
		file.close();

	return program;
}

void OpenCLUtils::BuildProgram(cl::Program& program, const std::vector<OpenCLDevice>& devices, const std::string &compile_options)
{
	std::vector<cl::Device> cl_devices;
	auto it = devices.cbegin();
	auto end = devices.cend();
	for (; it != end; ++it)
	{
		cl_devices.push_back(it->Device());
	}
	auto error = program.build(cl_devices, compile_options.c_str());
}

cl::Kernel OpenCLUtils::GetKernel(cl::Program& program, const std::string& kernel_name)
{
	auto error = 0;
	auto kernel = cl::Kernel(program, kernel_name.c_str(), &error);
	return kernel;
}

std::string OpenCLUtils::GetBuildLog(cl::Program& program)
{
	std::string log = "";
	auto it = _devices.begin();
	auto end = _devices.end();
	for (; it != end; ++it)
	{
		log += program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(it->Device());
	}
	return log;
}