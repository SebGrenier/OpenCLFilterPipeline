#pragma once

#include "BaseFilter.h"
#include "OpenCLUtils.h"

class MeanFilter : public BaseFilter
{
public:
	MeanFilter() : BaseFilter() {}
	virtual ~MeanFilter() {}

	virtual void Apply(const Image &input, Image &output) = 0;
	virtual void BuildParameterList() override;
	virtual std::string Name() const = 0;
};

class MeanFilter_CPU : public MeanFilter
{
public:
	MeanFilter_CPU();
	~MeanFilter_CPU();

	virtual void Apply(const Image &input, Image &output) override;
	virtual std::string Name() const override { return "Mean Filter CPU"; }

private:
};

class CMeanFilter_GPU : public MeanFilter
{
public:
	CMeanFilter_GPU(void);
	virtual ~CMeanFilter_GPU(void);

	virtual void Apply(const Image &input, Image &output) override;
	virtual std::string Name() const override { return "Mean Filter GPU 1 Pass"; }

private:
	cl::Program m_Program;
	cl::Kernel m_KernelH;
};

class CMeanFilter_GPU1 : public MeanFilter
{
public:
	CMeanFilter_GPU1(void);
	virtual ~CMeanFilter_GPU1(void);

	virtual void Apply(const Image &input, Image &output) override;
	virtual std::string Name() const override { return "Mean Filter GPU 2 Pass"; }

private:
	cl::Program m_Program;
	cl::Kernel m_KernelH;
	cl::Kernel m_KernelV;
};

class CMeanFilter_GPU2 : public MeanFilter
{
public:
	CMeanFilter_GPU2(void);
	virtual ~CMeanFilter_GPU2(void);

	virtual void Apply(const Image &input, Image &output) override;
	virtual std::string Name() const override { return "Mean Filter GPU 2 Pass No Local"; }

private:
	cl::Program m_Program;
	cl::Kernel m_KernelH;
	cl::Kernel m_KernelV;
};

class CMeanFilter_GPUTranspose : public MeanFilter
{
public:
	CMeanFilter_GPUTranspose(void);
	virtual ~CMeanFilter_GPUTranspose(void);

	virtual void Apply(const Image &input, Image &output) override;
	virtual std::string Name() const override { return "Mean Filter GPU Transpose"; }

private:
	cl::Program m_Program;
	cl::Kernel m_KernelH;
	cl::Kernel m_KernelV;
	cl::Kernel m_KernelT;
};

class CMeanFilter_GPUPad : public MeanFilter
{
public:
	CMeanFilter_GPUPad(void);
	virtual ~CMeanFilter_GPUPad(void);

	virtual void Apply(const Image &input, Image &output) override;
	virtual std::string Name() const override { return "Mean Filter GPU Padding"; }

private:
	cl::Program m_Program;
	cl::Kernel m_KernelH;
	cl::Kernel m_KernelV;
	cl::Kernel m_KernelZero;
};

class CMeanFilter_GPUPadTranspose : public MeanFilter
{
public:
	CMeanFilter_GPUPadTranspose(void);
	virtual ~CMeanFilter_GPUPadTranspose(void);

	virtual void Apply(const Image &input, Image &output) override;
	virtual std::string Name() const override { return "Mean Filter GPU Transpose + Padding"; }

private:
	cl::Program m_Program;
	cl::Kernel m_KernelH;
	cl::Kernel m_KernelV;
	cl::Kernel m_KernelT;
	cl::Kernel m_KernelZero;
};

class CMeanFilter_GPUPadTranspose_Aligned : public MeanFilter
{
public:
	CMeanFilter_GPUPadTranspose_Aligned(void);
	virtual ~CMeanFilter_GPUPadTranspose_Aligned(void);

	virtual void Apply(const Image &input, Image &output) override;
	virtual std::string Name() const override { return "Mean Filter GPU Alignment"; }

private:
	cl::Program m_Program;
	cl::Kernel m_KernelH;
	cl::Kernel m_KernelV;
	cl::Kernel m_KernelT;
	cl::Kernel m_KernelZero;
};