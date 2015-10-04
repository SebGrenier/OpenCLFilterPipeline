#pragma once
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 110
#define CL_HPP_MINIMUM_OPENCL_VERSION 110
#include <string>
#include <vector>
#include "cl2.hpp"

class OpenCLDevice
{
public:
	OpenCLDevice(cl::Device device, cl::CommandQueue queue, cl::Context context);
	~OpenCLDevice();

	inline const std::string& Name() const { return _name; }
	inline cl::Device Device() { return _device; }
	inline cl::CommandQueue CommandQueue() { return _command_queue; }

private:
	std::string _name;
	cl::Device _device;
	cl::CommandQueue _command_queue;
	cl::Context _context;
};

class OpenCLUtils
{
public:
	static OpenCLUtils* Instance();

	static bool CheckOpenCLError(cl_int returnCode, const std::string &ErrorString);
	bool InitOpenCL();
	inline bool IsInitialized() const { return _is_initialized; }

	OpenCLDevice& GetCurrentDevice();
	inline int GetCurrentDeviceIndex() const { return _current_device_index; }
	void SetCurrentDeviceIndex(int index);

	inline std::vector<OpenCLDevice>& Devices() { return _devices; }

private:
	OpenCLUtils();
	~OpenCLUtils();

	static OpenCLUtils *_instance;

	bool _is_initialized;
	std::vector<cl::Platform> _platforms;
	std::vector<OpenCLDevice> _devices;
	int _current_device_index;
};
