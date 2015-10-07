#include "MeanFilters.h"
#include <OpenCLUtils.h>
#include <iostream>
#include <QDebug>

#include "MathUtils.h"

void MeanFilter::BuildParameterList()
{
	_parameters->AddParameter(new CFilterInt(1, "Radius", "Radius of the kernel", 1, 9999));
}

MeanFilter_CPU::MeanFilter_CPU()
	: MeanFilter()
{
}

MeanFilter_CPU::~MeanFilter_CPU()
{
}

void MeanFilter_CPU::Apply(const Image &input, Image &output)
{
	const int radius = CFilterParameterInterpreter<int>::Convert(_parameters->GetParameter("Radius"));
	const int width = input.Width();
	const int height = input.Height();
	const int depth = input.Depth();
	const unsigned char *data = input.Data();
	unsigned char *out_data = output.Data();

	int i = 0, j = 0, ii = 0, jj = 0;
	int borneInfx = 0, borneSupx = radius + 1;

	std::vector<int> accuYMemoryR; accuYMemoryR.resize(radius + 1, 0);
	std::vector<int> compteurYMemoryR; compteurYMemoryR.resize(radius + 1, 0);
	std::vector<int> accuYMemoryG; accuYMemoryG.resize(radius + 1, 0);
	std::vector<int> compteurYMemoryG; compteurYMemoryG.resize(radius + 1, 0);
	std::vector<int> accuYMemoryB; accuYMemoryB.resize(radius + 1, 0);
	std::vector<int> compteurYMemoryB; compteurYMemoryB.resize(radius + 1, 0);

	int index = 0;

	int nbpt_R = 0;
	int nbpttemp_R = 0;
	int sommetemp_R = 0;
	int somme_R = 0;
	int compteurligneY_R = 0;
	int sommeligneY_R = 0;
	int valtemp_R = 0;

	int nbpt_G = 0;
	int nbpttemp_G = 0;
	int sommetemp_G = 0;
	int somme_G = 0;
	int compteurligneY_G = 0;
	int sommeligneY_G = 0;
	int valtemp_G = 0;

	int nbpt_B = 0;
	int nbpttemp_B = 0;
	int sommetemp_B = 0;
	int somme_B = 0;
	int compteurligneY_B = 0;
	int sommeligneY_B = 0;
	int valtemp_B = 0;


	// Initialisation
	for (jj = 0; jj < radius + 1; jj++){
		for (ii = 0; ii < radius + 1; ii++){
			index = (jj * width + ii) * depth;

			valtemp_R = int(data[index]);
			accuYMemoryR[jj] = accuYMemoryR[jj] + valtemp_R; // Somme des valeurs horizontalement
			compteurYMemoryR[jj] = compteurYMemoryR[jj] + 1;
			somme_R += valtemp_R;
			nbpt_R += 1;

			valtemp_G = int(data[index + 1]);
			accuYMemoryG[jj] = accuYMemoryG[jj] + valtemp_G; // Somme des valeurs horizontalement
			compteurYMemoryG[jj] = compteurYMemoryG[jj] + 1;
			somme_G += valtemp_G;
			nbpt_G += 1;

			valtemp_B = int(data[index + 2]);
			accuYMemoryB[jj] = accuYMemoryB[jj] + valtemp_B; // Somme des valeurs horizontalement
			compteurYMemoryB[jj] = compteurYMemoryB[jj] + 1;
			somme_B += valtemp_B;
			nbpt_B += 1;
		}
	}

	std::vector<int> compteurY_R; compteurY_R.resize(radius + 1, 0);
	std::vector<int> accuY_R; accuY_R.resize(radius + 1, 0);

	std::vector<int> compteurY_G; compteurY_G.resize(radius + 1, 0);
	std::vector<int> accuY_G; accuY_G.resize(radius + 1, 0);

	std::vector<int> compteurY_B; compteurY_B.resize(radius + 1, 0);
	std::vector<int> accuY_B; accuY_B.resize(radius + 1, 0);

	// Boucle principale
	for (i = 0; i < width; ++i){
		accuY_R = accuYMemoryR;
		compteurY_R = compteurYMemoryR;
		accuY_G = accuYMemoryG;
		compteurY_G = compteurYMemoryG;
		accuY_B = accuYMemoryB;
		compteurY_B = compteurYMemoryB;

		sommetemp_R = somme_R;
		nbpttemp_R = nbpt_R;
		sommetemp_G = somme_G;
		nbpttemp_G = nbpt_G;
		sommetemp_B = somme_B;
		nbpttemp_B = nbpt_B;

		index = 0;

		if (nbpttemp_R != 0){
			out_data[i * depth] = (unsigned char)((double)sommetemp_R / (double)nbpttemp_R);
		}
		if (nbpttemp_G != 0){
			out_data[i * depth + 1] = (unsigned char)((double)sommetemp_G / (double)nbpttemp_G);
		}
		if (nbpttemp_B != 0){
			out_data[i * depth + 2] = (unsigned char)((double)sommetemp_B / (double)nbpttemp_B);
		}

		borneInfx = (i - radius > 0) ? (i - radius) : 0; // ATTENTION
		borneSupx = (i + radius + 1 < width) ? (i + radius + 1) : width;


		for (j = 1; j < height; ++j){

			jj = j + radius;

			// Si on peut entrer une nouvelle ligne
			if (jj < height){
				for (ii = borneInfx; ii < borneSupx; ii++){
					sommeligneY_R += int(data[(jj * width + ii) * depth]);
					compteurligneY_R++;
					sommeligneY_G += int(data[(jj * width + ii) * depth + 1]);
					compteurligneY_G++;
					sommeligneY_B += int(data[(jj * width + ii) * depth + 2]);
					compteurligneY_B++;
				}
				sommetemp_R += sommeligneY_R;
				nbpttemp_R += compteurligneY_R;
				accuY_R.push_back(sommeligneY_R);
				compteurY_R.push_back(compteurligneY_R);
				sommeligneY_R = 0;
				compteurligneY_R = 0;

				sommetemp_G += sommeligneY_G;
				nbpttemp_G += compteurligneY_G;
				accuY_G.push_back(sommeligneY_G);
				compteurY_G.push_back(compteurligneY_G);
				sommeligneY_G = 0;
				compteurligneY_G = 0;

				sommetemp_B += sommeligneY_B;
				nbpttemp_B += compteurligneY_B;
				accuY_B.push_back(sommeligneY_B);
				compteurY_B.push_back(compteurligneY_B);
				sommeligneY_B = 0;
				compteurligneY_B = 0;
			}

			// Si on ne touche plus le bord
			if ((j - radius) > 0){
				sommetemp_R -= accuY_R[index];
				nbpttemp_R -= compteurY_R[index];

				sommetemp_G -= accuY_G[index];
				nbpttemp_G -= compteurY_G[index];

				sommetemp_B -= accuY_B[index];
				nbpttemp_B -= compteurY_B[index];

				index++;
			}

			if (nbpttemp_R != 0){
				out_data[(j * width + i) * depth] = (unsigned char)((double)sommetemp_R / (double)nbpttemp_R);
			}
			if (nbpttemp_G != 0){
				out_data[(j * width + i) * depth + 1] = (unsigned char)((double)sommetemp_G / (double)nbpttemp_G);
			}
			if (nbpttemp_B != 0){
				out_data[(j * width + i) * depth + 2] = (unsigned char)((double)sommetemp_B / (double)nbpttemp_B);
			}
		}


		// Reinitialisation
		accuY_R.resize(radius + 1, 0);
		compteurY_R.resize(radius + 1, 0);
		accuY_G.resize(radius + 1, 0);
		compteurY_G.resize(radius + 1, 0);
		accuY_B.resize(radius + 1, 0);
		compteurY_B.resize(radius + 1, 0);
		index = 0;

		if (i - radius >= 0){
			for (jj = 0; jj < (radius + 1) && jj < height; jj++){
				valtemp_R = int(data[(jj * width + borneInfx) * depth]);
				accuYMemoryR[jj] -= valtemp_R;// accuYMemory[jj] - valtemp;
				compteurYMemoryR[jj]--;// = compteurYMemory[jj] - 1;
				somme_R -= valtemp_R;
				nbpt_R--;

				valtemp_G = int(data[(jj * width + borneInfx) * depth + 1]);
				accuYMemoryG[jj] -= valtemp_G;// accuYMemory[jj] - valtemp;
				compteurYMemoryG[jj]--;// = compteurYMemory[jj] - 1;
				somme_G -= valtemp_G;
				nbpt_G--;

				valtemp_B = int(data[(jj * width + borneInfx) * depth + 2]);
				accuYMemoryB[jj] -= valtemp_B;// accuYMemory[jj] - valtemp;
				compteurYMemoryB[jj]--;// = compteurYMemory[jj] - 1;
				somme_B -= valtemp_B;
				nbpt_B--;
			}
		}

		if (borneSupx < width){
			for (jj = 0; jj < (radius + 1) && jj < height; jj++){
				valtemp_R = int(data[(jj * width + borneSupx) * depth]);
				accuYMemoryR[jj] += valtemp_R;// accuYMemory[jj] + valtemp;
				compteurYMemoryR[jj]++;// = compteurYMemory[jj] + 1;
				somme_R += valtemp_R;
				nbpt_R++;

				valtemp_G = int(data[(jj * width + borneSupx) * depth + 1]);
				accuYMemoryG[jj] += valtemp_G;// accuYMemory[jj] + valtemp;
				compteurYMemoryG[jj]++;// = compteurYMemory[jj] + 1;
				somme_G += valtemp_G;
				nbpt_G++;

				valtemp_B = int(data[(jj * width + borneSupx) * depth + 2]);
				accuYMemoryB[jj] += valtemp_B;// accuYMemory[jj] + valtemp;
				compteurYMemoryB[jj]++;// = compteurYMemory[jj] + 1;
				somme_B += valtemp_B;
				nbpt_B++;
			}
		}
	}
}

CMeanFilter_GPU::CMeanFilter_GPU(void)
	: MeanFilter()
{
	try
	{
		auto current_context = OpenCLUtils::Instance()->GetCurrentDevice().Context();
		m_Program = OpenCLUtils::Instance()->GetProgram(current_context, "MeanFilters.cl");
		OpenCLUtils::Instance()->BuildProgram(m_Program, OpenCLUtils::Instance()->Devices(), "");
	}
	catch (std::exception &err)
	{
		//std::cout << m_Program->GetBuildLog() << std::endl;
		//throw Err;
		qDebug() << "Error" << QString::fromStdString(OpenCLUtils::Instance()->GetBuildLog(m_Program));
	}

	m_KernelH = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilter_WithMask_1");
}

CMeanFilter_GPU::~CMeanFilter_GPU(void)
{
}

void CMeanFilter_GPU::Apply(const Image& input, Image& output)
{
	const int radius = CFilterParameterInterpreter<int>::Convert(_parameters->GetParameter("Radius"));
	const int width = input.Width();
	const int height = input.Height();
	const int NbPixel = width * height;
	const int depth = input.Depth();
	const unsigned char *image_data = input.Data();
	unsigned char *output_data = output.Data();

	//int *TempPix = new int[ NbPixel * 2 ];
	unsigned char *TempMask = new unsigned char[NbPixel];
	//memset( TempPix, 0, NbPixel * 2 * sizeof( int ) );
	memset(TempMask, 255, NbPixel * sizeof(unsigned char));

	auto current_device = OpenCLUtils::Instance()->GetCurrentDevice();
	const cl::Context Context = current_device.Context();

	cl_int Error;
	cl_mem_flags InOutMemFlags = CL_MEM_READ_WRITE;
	cl_mem_flags InMemFlags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
	cl_mem_flags OutMemFlags = CL_MEM_WRITE_ONLY;

	// If you ever change the local size, don't forget to change the size in the kernel
	// Global size must be a multiple of local size
	unsigned int GlobalSizeY = height;
	unsigned int GlobalSizeX = width;

	cl::Buffer ImBuffer(Context, InMemFlags, NbPixel * depth * sizeof(unsigned char), (void*)image_data, &Error);
	cl::Buffer MaskBuffer(Context, InMemFlags, NbPixel * sizeof(unsigned char), (void*)TempMask, &Error);
	cl::Buffer OutBuffer(Context, OutMemFlags, NbPixel * depth * sizeof(unsigned char), 0, &Error);

	cl::CommandQueue Queue = current_device.CommandQueue();
	cl::Event Event;

	// Horizontal pass
	int Index = 0;
	m_KernelH.setArg(Index++, ImBuffer);
	m_KernelH.setArg(Index++, MaskBuffer);
	m_KernelH.setArg(Index++, OutBuffer);
	m_KernelH.setArg(Index++, height);
	m_KernelH.setArg(Index++, width);
	m_KernelH.setArg(Index++, radius);

	Queue.enqueueNDRangeKernel(m_KernelH, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NullRange, 0, &Event);
	Event.wait();

	Queue.enqueueReadBuffer(OutBuffer, true, 0, NbPixel * depth * sizeof(unsigned char), (void*)output_data, 0, &Event);
	Event.wait();

	Queue.finish();

	delete[] TempMask;
}

CMeanFilter_GPU1::CMeanFilter_GPU1(void)
	: MeanFilter()
{
	try
	{
		auto current_context = OpenCLUtils::Instance()->GetCurrentDevice().Context();
		m_Program = OpenCLUtils::Instance()->GetProgram(current_context, "MeanFilters.cl");
		OpenCLUtils::Instance()->BuildProgram(m_Program, OpenCLUtils::Instance()->Devices(), "");
	}
	catch (std::exception &err)
	{
		//std::cout << m_Program->GetBuildLog() << std::endl;
		//throw Err;
		std::cout << "Error" << std::endl;
	}

	m_KernelH = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilter_WithMask_H");
	m_KernelV = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilter_WithMask_V");
}

CMeanFilter_GPU1::~CMeanFilter_GPU1(void)
{
}

void CMeanFilter_GPU1::Apply(const Image& input, Image& output)
{
	const int radius = CFilterParameterInterpreter<int>::Convert(_parameters->GetParameter("Radius"));
	const int width = input.Width();
	const int height = input.Height();
	const int NbPixel = width * height;
	const int depth = input.Depth();
	const unsigned char *image_data = input.Data();
	unsigned char *output_data = output.Data();

	//int *TempPix = new int[ NbPixel * 2 ];
	unsigned char *TempMask = new unsigned char[NbPixel];
	//memset( TempPix, 0, NbPixel * 2 * sizeof( int ) );
	memset(TempMask, 255, NbPixel * sizeof(unsigned char));

	auto current_device = OpenCLUtils::Instance()->GetCurrentDevice();
	const cl::Context Context = current_device.Context();

	cl_int Error;
	cl_mem_flags InOutMemFlags = CL_MEM_READ_WRITE;
	cl_mem_flags InMemFlags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
	cl_mem_flags OutMemFlags = CL_MEM_WRITE_ONLY;

	// If you ever change the local size, don't forget to change the size in the kernel
	unsigned int LocalSizeY = 1;
	unsigned int LocalSizeX = 32;
	// Global size must be a multiple of local size
	unsigned int GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
	GlobalSizeY *= LocalSizeY;
	unsigned int GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
	GlobalSizeX *= LocalSizeX;

	cl::Buffer ImBuffer(Context, InMemFlags, NbPixel * depth * sizeof(unsigned char), (void*)image_data, &Error);
	cl::Buffer MaskBuffer(Context, InMemFlags, NbPixel * sizeof(unsigned char), (void*)TempMask, &Error);
	cl::Buffer ImTempBuffer(Context,
		InOutMemFlags, NbPixel * depth * 2 * sizeof(int),
		0, &Error);
	cl::Buffer OutBuffer(Context, OutMemFlags, NbPixel * depth * sizeof(unsigned char), 0, &Error);

	cl::CommandQueue Queue = current_device.CommandQueue();
	cl::Event Event;

	// Horizontal pass
	int Index = 0;
	m_KernelH.setArg(Index++, ImBuffer);
	m_KernelH.setArg(Index++, MaskBuffer);
	m_KernelH.setArg(Index++, ImTempBuffer);
	m_KernelH.setArg(Index++, height);
	m_KernelH.setArg(Index++, width);
	m_KernelH.setArg(Index++, radius);

	Queue.enqueueNDRangeKernel(m_KernelH, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NDRange(LocalSizeX, LocalSizeY), 0, &Event);
	Event.wait();


	// Vertical pass
	LocalSizeY = 32;
	LocalSizeX = 1;
	// Global size must be a multiple of local size
	GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
	GlobalSizeY *= LocalSizeY;
	GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
	GlobalSizeX *= LocalSizeX;

	Index = 0;
	m_KernelV.setArg(Index++, ImTempBuffer);
	m_KernelV.setArg(Index++, MaskBuffer);
	m_KernelV.setArg(Index++, OutBuffer);
	m_KernelV.setArg(Index++, height);
	m_KernelV.setArg(Index++, width);
	m_KernelV.setArg(Index++, radius);

	Queue.enqueueNDRangeKernel(m_KernelV, cl::NullRange, cl::NDRange(GlobalSizeY, GlobalSizeX), cl::NDRange(LocalSizeY, LocalSizeX), 0, &Event);
	Event.wait();

	Queue.enqueueReadBuffer(OutBuffer, true, 0, NbPixel * depth * sizeof(unsigned char), (void*)output_data, 0, &Event);
	Event.wait();

	Queue.finish();

	delete[] TempMask;
}

CMeanFilter_GPU2::CMeanFilter_GPU2(void)
	: MeanFilter()
{
	try
	{
		auto current_context = OpenCLUtils::Instance()->GetCurrentDevice().Context();
		m_Program = OpenCLUtils::Instance()->GetProgram(current_context, "MeanFilters.cl");
		OpenCLUtils::Instance()->BuildProgram(m_Program, OpenCLUtils::Instance()->Devices(), "");
	}
	catch (std::exception &err)
	{
		//std::cout << m_Program->GetBuildLog() << std::endl;
		//throw Err;
		std::cout << "Error" << std::endl;
	}

	m_KernelH = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilter_WithMask_H_NoLocal");
	m_KernelV = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilter_WithMask_V_NoLocal");
}

CMeanFilter_GPU2::~CMeanFilter_GPU2(void)
{
}

void CMeanFilter_GPU2::Apply(const Image& input, Image& output)
{
	const int radius = CFilterParameterInterpreter<int>::Convert(_parameters->GetParameter("Radius"));
	const int width = input.Width();
	const int height = input.Height();
	const int NbPixel = width * height;
	const int depth = input.Depth();
	const unsigned char *image_data = input.Data();
	unsigned char *output_data = output.Data();

	//int *TempPix = new int[ NbPixel * 2 ];
	unsigned char *TempMask = new unsigned char[NbPixel];
	//memset( TempPix, 0, NbPixel * 2 * sizeof( int ) );
	memset(TempMask, 255, NbPixel * sizeof(unsigned char));

	auto current_device = OpenCLUtils::Instance()->GetCurrentDevice();
	const cl::Context Context = current_device.Context();

	cl_int Error;
	cl_mem_flags InOutMemFlags = CL_MEM_READ_WRITE;
	cl_mem_flags InMemFlags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
	cl_mem_flags OutMemFlags = CL_MEM_WRITE_ONLY;

	// If you ever change the local size, don't forget to change the size in the kernel
	unsigned int LocalSizeY = 1;
	unsigned int LocalSizeX = 32;
	// Global size must be a multiple of local size
	unsigned int GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
	GlobalSizeY *= LocalSizeY;
	unsigned int GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
	GlobalSizeX *= LocalSizeX;

	cl::Buffer ImBuffer(Context, InMemFlags, NbPixel * depth * sizeof(unsigned char), (void*)image_data, &Error);
	cl::Buffer MaskBuffer(Context, InMemFlags, NbPixel * sizeof(unsigned char), (void*)TempMask, &Error);
	cl::Buffer ImTempBuffer(Context,
		InOutMemFlags, NbPixel * depth * 2 * sizeof(int),
		0, &Error);
	cl::Buffer OutBuffer(Context, OutMemFlags, NbPixel * depth * sizeof(unsigned char), 0, &Error);

	cl::CommandQueue Queue = current_device.CommandQueue();
	cl::Event Event;

	// Horizontal pass
	int Index = 0;
	m_KernelH.setArg(Index++, ImBuffer);
	m_KernelH.setArg(Index++, MaskBuffer);
	m_KernelH.setArg(Index++, ImTempBuffer);
	m_KernelH.setArg(Index++, height);
	m_KernelH.setArg(Index++, width);
	m_KernelH.setArg(Index++, radius);

	Queue.enqueueNDRangeKernel(m_KernelH, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NDRange(LocalSizeX, LocalSizeY), 0, &Event);
	Event.wait();


	// Vertical pass
	LocalSizeY = 32;
	LocalSizeX = 1;
	// Global size must be a multiple of local size
	GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
	GlobalSizeY *= LocalSizeY;
	GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
	GlobalSizeX *= LocalSizeX;

	Index = 0;
	m_KernelV.setArg(Index++, ImTempBuffer);
	m_KernelV.setArg(Index++, MaskBuffer);
	m_KernelV.setArg(Index++, OutBuffer);
	m_KernelV.setArg(Index++, height);
	m_KernelV.setArg(Index++, width);
	m_KernelV.setArg(Index++, radius);

	Queue.enqueueNDRangeKernel(m_KernelV, cl::NullRange, cl::NDRange(GlobalSizeY, GlobalSizeX), cl::NDRange(LocalSizeY, LocalSizeX), 0, &Event);
	Event.wait();

	Queue.enqueueReadBuffer(OutBuffer, true, 0, NbPixel * depth * sizeof(unsigned char), (void*)output_data, 0, &Event);
	Event.wait();

	Queue.finish();

	delete[] TempMask;
}

CMeanFilter_GPUTranspose::CMeanFilter_GPUTranspose(void)
	: MeanFilter()
{
	try
	{
		auto current_context = OpenCLUtils::Instance()->GetCurrentDevice().Context();
		m_Program = OpenCLUtils::Instance()->GetProgram(current_context, "MeanFilters.cl");
		OpenCLUtils::Instance()->BuildProgram(m_Program, OpenCLUtils::Instance()->Devices(), "");
	}
	catch (std::exception &err)
	{
		//std::cout << m_Program->GetBuildLog() << std::endl;
		//throw Err;
		std::cout << "Error" << std::endl;
	}

	m_KernelH = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilter_WithMask_H");
	m_KernelV = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilter_WithMask_V_Transpose");
	m_KernelT = OpenCLUtils::Instance()->GetKernel(m_Program, "TempTranspose");
}

CMeanFilter_GPUTranspose::~CMeanFilter_GPUTranspose(void)
{
}

void CMeanFilter_GPUTranspose::Apply(const Image& input, Image& output)
{
	const int radius = CFilterParameterInterpreter<int>::Convert(_parameters->GetParameter("Radius"));
	const int width = input.Width();
	const int height = output.Height();
	const int NbPixel = width * height;
	const int depth = input.Depth();
	const uchar *image_data = input.Data();
	uchar *output_data = output.Data();

	//int *TempPix = new int[ NbPixel * 2 ];
	unsigned char *TempMask = new unsigned char[NbPixel];
	//memset( TempPix, 0, NbPixel * 2 * sizeof( int ) );
	memset(TempMask, 255, NbPixel * sizeof(unsigned char));

	auto current_device = OpenCLUtils::Instance()->GetCurrentDevice();
	const cl::Context Context = current_device.Context();

	cl_int Error;
	cl_mem_flags InOutMemFlags = CL_MEM_READ_WRITE;
	cl_mem_flags InMemFlags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
	cl_mem_flags OutMemFlags = CL_MEM_WRITE_ONLY;

	// If you ever change the local size, don't forget to change the size in the kernel
	unsigned int LocalSizeY = 1;
	unsigned int LocalSizeX = 32;
	// Global size must be a multiple of local size
	unsigned int GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
	GlobalSizeY *= LocalSizeY;
	unsigned int GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
	GlobalSizeX *= LocalSizeX;

	cl::Buffer ImBuffer(Context, InMemFlags, NbPixel * depth * sizeof(unsigned char), (void*)image_data, &Error);
	cl::Buffer MaskBuffer(Context, InMemFlags, NbPixel * sizeof(unsigned char), (void*)TempMask, &Error);
	cl::Buffer ImTempBuffer(Context,
		InOutMemFlags, NbPixel * depth * 2 * sizeof(int),
		0, &Error);
	cl::Buffer ImTemp2Buffer(Context, InOutMemFlags, NbPixel * depth * 2 * sizeof(int), 0, &Error);
	cl::Buffer OutBuffer(Context, OutMemFlags, NbPixel *depth* sizeof(unsigned char), 0, &Error);

	cl::CommandQueue Queue = current_device.CommandQueue();
	cl::Event Event;

	// Horizontal pass
	int Index = 0;
	m_KernelH.setArg(Index++, ImBuffer);
	m_KernelH.setArg(Index++, MaskBuffer);
	m_KernelH.setArg(Index++, ImTempBuffer);
	m_KernelH.setArg(Index++, height);
	m_KernelH.setArg(Index++, width);
	m_KernelH.setArg(Index++, radius);

	Queue.enqueueNDRangeKernel(m_KernelH, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NDRange(LocalSizeX, LocalSizeY), 0, &Event);
	Event.wait();

	// Transpose
	LocalSizeY = 16;
	LocalSizeX = 16;
	// Global size must be a multiple of local size
	GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
	GlobalSizeY *= LocalSizeY;
	GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
	GlobalSizeX *= LocalSizeX;

	Index = 0;
	m_KernelT.setArg(Index++, ImTempBuffer);
	m_KernelT.setArg(Index++, ImTemp2Buffer);
	m_KernelT.setArg(Index++, height);
	m_KernelT.setArg(Index++, width);

	Queue.enqueueNDRangeKernel(m_KernelT, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NDRange(LocalSizeX, LocalSizeY), 0, &Event);
	Event.wait();

	// Vertical pass
	LocalSizeY = 32;
	LocalSizeX = 1;
	// Global size must be a multiple of local size
	GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
	GlobalSizeY *= LocalSizeY;
	GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
	GlobalSizeX *= LocalSizeX;

	Index = 0;
	m_KernelV.setArg(Index++, ImTemp2Buffer);
	m_KernelV.setArg(Index++, MaskBuffer);
	m_KernelV.setArg(Index++, OutBuffer);
	m_KernelV.setArg(Index++, height);
	m_KernelV.setArg(Index++, width);
	m_KernelV.setArg(Index++, radius);

	Queue.enqueueNDRangeKernel(m_KernelV, cl::NullRange, cl::NDRange(GlobalSizeY, GlobalSizeX), cl::NDRange(LocalSizeY, LocalSizeX), 0, &Event);
	Event.wait();

	Queue.enqueueReadBuffer(OutBuffer, true, 0, NbPixel * depth * sizeof(unsigned char), (void*)output_data, 0, &Event);
	Event.wait();

	Queue.finish();

	//delete[] TempPix;
	delete[] TempMask;
}

CMeanFilter_GPUPad::CMeanFilter_GPUPad(void)
	: MeanFilter()
{
	try
	{
		auto current_context = OpenCLUtils::Instance()->GetCurrentDevice().Context();
		m_Program = OpenCLUtils::Instance()->GetProgram(current_context, "MeanFilters.cl");
		OpenCLUtils::Instance()->BuildProgram(m_Program, OpenCLUtils::Instance()->Devices(), "");
	}
	catch (std::exception &err)
	{
		//std::cout << m_Program->GetBuildLog() << std::endl;
		//throw Err;
		std::cout << "Error" << std::endl;
	}

	m_KernelH = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilterPad_H");
	m_KernelV = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilterPad_V");
	m_KernelZero = OpenCLUtils::Instance()->GetKernel(m_Program, "ZeroOut");
}

CMeanFilter_GPUPad::~CMeanFilter_GPUPad(void)
{
}

void CMeanFilter_GPUPad::Apply(const Image& input, Image& output)
{
	const int radius = CFilterParameterInterpreter<int>::Convert(_parameters->GetParameter("Radius"));
	const int GroupSize = 32;
	const int width = input.Width();
	const int height = input.Height();
	const int depth = input.Depth();
	const int NbPixel = width * height;
	const uchar *image_data = input.Data();
	uchar *output_data = output.Data();
	const int NewHeight = Utils::GetNextMultipleOf(height, GroupSize) + radius * 2 + GroupSize;
	const int NewWidth = Utils::GetNextMultipleOf(width, GroupSize) + radius * 2 + GroupSize;
	const int NewNbPixel = NewHeight * NewWidth;

	Image NewImage(NewWidth, NewHeight, depth);
	int *TempPix = new int[NewNbPixel * 2 * depth];
	unsigned char *TempMask = new unsigned char[NewNbPixel];
	Image NewOut(NewWidth, NewHeight, depth);
	memset(TempPix, 0, NewNbPixel * 2 * depth * sizeof(int));
	memset(TempMask, 0, NewNbPixel * sizeof(unsigned char));

	// Copy image into subrect
	int NewImageIndex = 0;
	int OldImageIndex = 0;
	int temp_val = 0;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			for (int k = 0; k < depth; ++k)
			{
				NewImage(i + radius,j + radius,k) = input(i,j,k);
			}
			TempMask[(i+radius) * NewWidth + j + radius] = 1;
		}
	}

	auto current_device = OpenCLUtils::Instance()->GetCurrentDevice();
	const cl::Context Context = current_device.Context();

	cl_int Error;
	cl_mem_flags InOutMemFlags = CL_MEM_READ_WRITE;
	cl_mem_flags InMemFlags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
	cl_mem_flags OutMemFlags = CL_MEM_WRITE_ONLY;

	// If you ever change the local size, don't forget to change the size in the kernel
	unsigned int LocalSizeY = 1;
	unsigned int LocalSizeX = GroupSize;
	// Global size must be a multiple of local size
	unsigned int GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
	GlobalSizeY *= LocalSizeY;
	unsigned int GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
	GlobalSizeX *= LocalSizeX;

	cl::Buffer ImBuffer(Context, InMemFlags, NewNbPixel * depth * sizeof(unsigned char), (void*)NewImage.Data(), &Error);
	cl::Buffer MaskBuffer(Context, InMemFlags, NewNbPixel * sizeof(unsigned char), (void*)TempMask, &Error);
	cl::Buffer ImTempBuffer(Context,
		InOutMemFlags, NewNbPixel * 2 * depth * sizeof(int),
		0, &Error);
	cl::Buffer OutBuffer(Context, OutMemFlags, NewNbPixel * depth * sizeof(unsigned char), 0, &Error);

	cl::CommandQueue Queue = current_device.CommandQueue();
	cl::Event Event;

	// Zero out temp buffer
	m_KernelZero.setArg(0, ImTempBuffer);
	Queue.enqueueNDRangeKernel(m_KernelZero, cl::NullRange, cl::NDRange(NewNbPixel), cl::NullRange, 0, &Event);
	Event.wait();

	// Horizontal pass
	int Index = 0;
	m_KernelH.setArg(Index++, ImBuffer);
	m_KernelH.setArg(Index++, MaskBuffer);
	m_KernelH.setArg(Index++, ImTempBuffer);
	m_KernelH.setArg(Index++, NewHeight);
	m_KernelH.setArg(Index++, NewWidth);
	m_KernelH.setArg(Index++, radius);

	Queue.enqueueNDRangeKernel(m_KernelH, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NDRange(LocalSizeX, LocalSizeY), 0, &Event);
	Event.wait();

	// Vertical pass
	LocalSizeY = GroupSize;
	LocalSizeX = 1;
	// Global size must be a multiple of local size
	GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
	GlobalSizeY *= LocalSizeY;
	GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
	GlobalSizeX *= LocalSizeX;

	Index = 0;
	m_KernelV.setArg(Index++, ImTempBuffer);
	m_KernelV.setArg(Index++, OutBuffer);
	m_KernelV.setArg(Index++, NewHeight);
	m_KernelV.setArg(Index++, NewWidth);
	m_KernelV.setArg(Index++, radius);

	Queue.enqueueNDRangeKernel(m_KernelV, cl::NullRange, cl::NDRange(GlobalSizeY, GlobalSizeX), cl::NDRange(LocalSizeY, LocalSizeX), 0, &Event);
	Event.wait();

	Queue.enqueueReadBuffer(OutBuffer, true, 0, NewNbPixel * depth * sizeof(unsigned char), (void*)NewOut.Data(), 0, &Event);
	Event.wait();

	Queue.finish();

	//PrintToFile( NewOut, (size_t)NewWidth, (size_t)NewHeight, "GPUPad.txt" );

	// Read Sub rect
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			for (int k = 0; k < depth; ++k)
			{
				output(i,j,k) = NewOut(i+radius, j+radius,k);
			}
		}
	}

	delete[] TempPix;
	delete[] TempMask;
}
// 
// CMeanFilter_GPUPadTranspose::CMeanFilter_GPUPadTranspose(void)
// 	: MeanFilter()
// {
// 	try
// 	{
// 		auto current_context = OpenCLUtils::Instance()->GetCurrentDevice().Context();
// 		m_Program = OpenCLUtils::Instance()->GetProgram(current_context, "MeanFilters.cl");
// 		OpenCLUtils::Instance()->BuildProgram(m_Program, OpenCLUtils::Instance()->Devices(), "");
// 	}
// 	catch (std::exception &err)
// 	{
// 		//std::cout << m_Program->GetBuildLog() << std::endl;
// 		//throw Err;
// 		std::cout << "Error" << std::endl;
// 	}
// 
// 	m_KernelH = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilterPad2_H");
// 	m_KernelV = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilterPad_V_Transpose");
// 	m_KernelT = OpenCLUtils::Instance()->GetKernel(m_Program, "TempTranspose");
// 	m_KernelZero = OpenCLUtils::Instance()->GetKernel(m_Program, "ZeroOut");
// }
// 
// CMeanFilter_GPUPadTranspose::~CMeanFilter_GPUPadTranspose(void)
// {
// }
// 
// void CMeanFilter_GPUPadTranspose::Run(void)
// {
// 	const int GroupSize = 32;
// 	const int width = m_ImageWidth;
// 	const int height = m_ImageHeight;
// 	const int NbPixel = width * height;
// 	const int NewHeight = GetNextMultipleOf(height, GroupSize) + m_Radius * 2 + GroupSize;
// 	const int NewWidth = GetNextMultipleOf(width, GroupSize) + m_Radius * 2 + GroupSize;
// 	const int NewNbPixel = NewHeight * NewWidth;
// 
// 	unsigned char *NewImageData = new unsigned char[NewNbPixel];
// 	unsigned char *TempMask = new unsigned char[NewNbPixel];
// 	unsigned char *NewOut = new unsigned char[NewNbPixel];
// 	memset(NewImageData, 0, NewNbPixel * sizeof(unsigned char));
// 	memset(TempMask, 0, NewNbPixel * sizeof(unsigned char));
// 
// 	// Copy image into subrect
// 	int NewImageIndex = 0;
// 	int OldImageIndex = 0;
// 	for (int i = 0; i < height; ++i)
// 	{
// 		NewImageIndex = (i + m_Radius) * NewWidth + m_Radius;
// 		for (int j = 0; j < width; ++j)
// 		{
// 			NewImageData[NewImageIndex] = m_ImageData[OldImageIndex];
// 			TempMask[NewImageIndex] = 1;
// 			++OldImageIndex;
// 			++NewImageIndex;
// 		}
// 	}
// 
// 	const cl::Context Context = Compute::CCompute::GetInstance()->GetContext();
// 
// 	cl_int Error;
// 	cl_mem_flags InOutMemFlags = CL_MEM_READ_WRITE;
// 	cl_mem_flags InMemFlags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
// 	cl_mem_flags OutMemFlags = CL_MEM_WRITE_ONLY;
// 
// 	// If you ever change the local size, don't forget to change the size in the kernel
// 	unsigned int LocalSizeY = 1;
// 	unsigned int LocalSizeX = GroupSize;
// 	// Global size must be a multiple of local size
// 	unsigned int GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
// 	GlobalSizeY *= LocalSizeY;
// 	unsigned int GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
// 	GlobalSizeX *= LocalSizeX;
// 
// 	cl::Buffer ImBuffer(Context, InMemFlags, NewNbPixel * sizeof(unsigned char), (void*)NewImageData, &Error);
// 	cl::Buffer MaskBuffer(Context, InMemFlags, NewNbPixel * sizeof(unsigned char), (void*)TempMask, &Error);
// 	cl::Buffer ImTempBuffer(Context,
// 		InOutMemFlags, NewNbPixel * 2 * sizeof(int),
// 		0, &Error);
// 	cl::Buffer ImTempBuffer2(Context,
// 		InOutMemFlags, NewNbPixel * 2 * sizeof(int),
// 		0, &Error);
// 	cl::Buffer OutBuffer(Context, OutMemFlags, NewNbPixel * sizeof(unsigned char), 0, &Error);
// 
// 	cl::CommandQueue Queue = Compute::CCompute::GetInstance()->GetCommandQueue();
// 	cl::Event Event;
// 
// 	// Zero out temp buffers
// 	m_KernelZero.setArg(0, ImTempBuffer);
// 	Queue.enqueueNDRangeKernel(m_KernelZero, cl::NullRange, cl::NDRange(NewNbPixel), cl::NullRange, 0, &Event);
// 	Event.wait();
// 	m_KernelZero.setArg(0, ImTempBuffer2);
// 	Queue.enqueueNDRangeKernel(m_KernelZero, cl::NullRange, cl::NDRange(NewNbPixel), cl::NullRange, 0, &Event);
// 	Event.wait();
// 
// 	// Horizontal pass
// 	int Index = 0;
// 	m_KernelH.setArg(Index++, ImBuffer);
// 	m_KernelH.setArg(Index++, MaskBuffer);
// 	m_KernelH.setArg(Index++, ImTempBuffer);
// 	m_KernelH.setArg(Index++, NewHeight);
// 	m_KernelH.setArg(Index++, NewWidth);
// 	m_KernelH.setArg(Index++, m_Radius);
// 
// 	Queue.enqueueNDRangeKernel(m_KernelH, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NDRange(LocalSizeX, LocalSizeY), 0, &Event);
// 	Event.wait();
// 
// 	// Transpose
// 	LocalSizeY = 16;
// 	LocalSizeX = 16;
// 	// Global size must be a multiple of local size
// 	GlobalSizeY = (NewHeight + LocalSizeY - 1) / LocalSizeY;
// 	GlobalSizeY *= LocalSizeY;
// 	GlobalSizeX = (NewWidth + LocalSizeX - 1) / LocalSizeX;
// 	GlobalSizeX *= LocalSizeX;
// 
// 	Index = 0;
// 	m_KernelT.setArg(Index++, ImTempBuffer);
// 	m_KernelT.setArg(Index++, ImTempBuffer2);
// 	m_KernelT.setArg(Index++, NewHeight);
// 	m_KernelT.setArg(Index++, NewWidth);
// 
// 	Queue.enqueueNDRangeKernel(m_KernelT, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NDRange(LocalSizeX, LocalSizeY), 0, &Event);
// 	Event.wait();
// 
// 	// Vertical pass
// 	LocalSizeY = GroupSize;
// 	LocalSizeX = 1;
// 	// Global size must be a multiple of local size
// 	GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
// 	GlobalSizeY *= LocalSizeY;
// 	GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
// 	GlobalSizeX *= LocalSizeX;
// 
// 	Index = 0;
// 	m_KernelV.setArg(Index++, ImTempBuffer2);
// 	m_KernelV.setArg(Index++, OutBuffer);
// 	m_KernelV.setArg(Index++, NewHeight);
// 	m_KernelV.setArg(Index++, NewWidth);
// 	m_KernelV.setArg(Index++, m_Radius);
// 
// 	Queue.enqueueNDRangeKernel(m_KernelV, cl::NullRange, cl::NDRange(GlobalSizeY, GlobalSizeX), cl::NDRange(LocalSizeY, LocalSizeX), 0, &Event);
// 	Event.wait();
// 
// 	Queue.enqueueReadBuffer(OutBuffer, true, 0, NewNbPixel * sizeof(unsigned char), (void*)NewOut, 0, &Event);
// 	Event.wait();
// 
// 	Queue.finish();
// 
// 	//PrintToFile( NewOut, (size_t)NewWidth, (size_t)NewHeight, "GPUPadTranspose.txt" );
// 
// 	// Read Sub rect
// 	for (int i = 0; i < height; ++i)
// 	{
// 		for (int j = 0; j < width; ++j)
// 		{
// 			int NewImageIndex = ((i + m_Radius) * NewWidth) + m_Radius + j;
// 			int OldImageIndex = i * width + j;
// 			m_ResultData[OldImageIndex] = NewOut[NewImageIndex];
// 		}
// 	}
// 
// 	delete[] NewImageData;
// 	delete[] NewOut;
// 	delete[] TempMask;
// }
// 
// CMeanFilter_GPUPadTranspose_Aligned::CMeanFilter_GPUPadTranspose_Aligned(void)
// 	: MeanFilter()
// {
// 	try
// 	{
// 		auto current_context = OpenCLUtils::Instance()->GetCurrentDevice().Context();
// 		m_Program = OpenCLUtils::Instance()->GetProgram(current_context, "MeanFilters.cl");
// 		OpenCLUtils::Instance()->BuildProgram(m_Program, OpenCLUtils::Instance()->Devices(), "");
// 	}
// 	catch (std::exception &err)
// 	{
// 		//std::cout << m_Program->GetBuildLog() << std::endl;
// 		//throw Err;
// 		std::cout << "Error" << std::endl;
// 	}
// 
// 	m_KernelH = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilterPadAligned_H");
// 	m_KernelV = OpenCLUtils::Instance()->GetKernel(m_Program, "MeanFilterPadAligned_V_Transpose");
// 	m_KernelT = OpenCLUtils::Instance()->GetKernel(m_Program, "TempTranspose2");
// 	m_KernelZero = OpenCLUtils::Instance()->GetKernel(m_Program, "ZeroOut");
// }
// 
// CMeanFilter_GPUPadTranspose_Aligned::~CMeanFilter_GPUPadTranspose_Aligned(void)
// {
// }
// 
// void CMeanFilter_GPUPadTranspose_Aligned::Run(void)
// {
// 	const int GroupSize = 32;
// 	const int ByteAlign = 32;
// 	const int width = m_ImageWidth;
// 	const int height = m_ImageHeight;
// 	int NewHeight = GetNextMultipleOf(height, GroupSize) + m_Radius * 2 + GroupSize;
// 	NewHeight += ByteAlign; // Add 32 bytes to align before each column start (since its char, 32 bytes is 32 chars)
// 	NewHeight = GetNextMultipleOf(NewHeight, ByteAlign); // Align the end so the start of the next column is also aligned
// 	int NewWidth = GetNextMultipleOf(width, GroupSize) + m_Radius * 2 + GroupSize;
// 	NewWidth += ByteAlign; // Add 32 bytes to align before each row start (since its char, 32 bytes is 32 chars)
// 	NewWidth = GetNextMultipleOf(NewWidth, ByteAlign); // Align the end so the start of the next row is also aligned
// 	const int NewNbPixel = NewHeight * NewWidth;
// 
// 	unsigned char *NewImageData = new unsigned char[NewNbPixel];
// 	unsigned char *TempMask = new unsigned char[NewNbPixel];
// 	unsigned char *NewOut = new unsigned char[NewNbPixel];
// 	memset(NewImageData, 0, NewNbPixel * sizeof(unsigned char));
// 	memset(TempMask, 0, NewNbPixel * sizeof(unsigned char));
// 
// 	// Copy image into subrect
// 	int NewImageIndex = 0;
// 	int OldImageIndex = 0;
// 	for (int i = 0; i < height; ++i)
// 	{
// 		NewImageIndex = (i + m_Radius + ByteAlign) * NewWidth + m_Radius + ByteAlign;
// 		for (int j = 0; j < width; ++j)
// 		{
// 			NewImageData[NewImageIndex] = m_ImageData[OldImageIndex];
// 			TempMask[NewImageIndex] = 1;
// 			++OldImageIndex;
// 			++NewImageIndex;
// 		}
// 	}
// 
// 	const cl::Context Context = Compute::CCompute::GetInstance()->GetContext();
// 
// 	cl_int Error;
// 	cl_mem_flags InOutMemFlags = CL_MEM_READ_WRITE;
// 	cl_mem_flags InMemFlags = CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR;
// 	cl_mem_flags OutMemFlags = CL_MEM_WRITE_ONLY;
// 
// 	// If you ever change the local size, don't forget to change the size in the kernel
// 	unsigned int LocalSizeY = 1;
// 	unsigned int LocalSizeX = GroupSize;
// 	// Global size must be a multiple of local size
// 	unsigned int GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
// 	GlobalSizeY *= LocalSizeY;
// 	unsigned int GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
// 	GlobalSizeX *= LocalSizeX;
// 
// 	cl::Buffer ImBuffer(Context, InMemFlags, NewNbPixel * sizeof(unsigned char), (void*)NewImageData, &Error);
// 	cl::Buffer MaskBuffer(Context, InMemFlags, NewNbPixel * sizeof(unsigned char), (void*)TempMask, &Error);
// 	cl::Buffer ImTempBuffer(Context,
// 		InOutMemFlags, NewNbPixel * 2 * sizeof(int),
// 		0, &Error);
// 	cl::Buffer ImTempBuffer2(Context,
// 		InOutMemFlags, NewNbPixel * 2 * sizeof(int),
// 		0, &Error);
// 	cl::Buffer OutBuffer(Context, OutMemFlags, NewNbPixel * sizeof(unsigned char), 0, &Error);
// 
// 	cl::CommandQueue Queue = Compute::CCompute::GetInstance()->GetCommandQueue();
// 	cl::Event Event;
// 
// 	// Zero out temp buffers
// 	m_KernelZero.setArg(0, ImTempBuffer);
// 	Queue.enqueueNDRangeKernel(m_KernelZero, cl::NullRange, cl::NDRange(NewNbPixel), cl::NullRange, 0, &Event);
// 	Event.wait();
// 	m_KernelZero.setArg(0, ImTempBuffer2);
// 	Queue.enqueueNDRangeKernel(m_KernelZero, cl::NullRange, cl::NDRange(NewNbPixel), cl::NullRange, 0, &Event);
// 	Event.wait();
// 
// 	// Horizontal pass
// 	int Index = 0;
// 	m_KernelH.setArg(Index++, ImBuffer);
// 	m_KernelH.setArg(Index++, MaskBuffer);
// 	m_KernelH.setArg(Index++, ImTempBuffer);
// 	m_KernelH.setArg(Index++, NewHeight);
// 	m_KernelH.setArg(Index++, NewWidth);
// 	m_KernelH.setArg(Index++, m_Radius);
// 	m_KernelH.setArg(Index++, ByteAlign);
// 
// 	Queue.enqueueNDRangeKernel(m_KernelH, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NDRange(LocalSizeX, LocalSizeY), 0, &Event);
// 	Event.wait();
// 
// 	// Transpose
// 	LocalSizeY = 16;
// 	LocalSizeX = 16;
// 	// Global size must be a multiple of local size
// 	GlobalSizeY = (NewHeight + LocalSizeY - 1) / LocalSizeY;
// 	GlobalSizeY *= LocalSizeY;
// 	GlobalSizeX = (NewWidth + LocalSizeX - 1) / LocalSizeX;
// 	GlobalSizeX *= LocalSizeX;
// 
// 	Index = 0;
// 	m_KernelT.setArg(Index++, ImTempBuffer);
// 	m_KernelT.setArg(Index++, ImTempBuffer2);
// 	m_KernelT.setArg(Index++, NewHeight);
// 	m_KernelT.setArg(Index++, NewWidth);
// 
// 	Queue.enqueueNDRangeKernel(m_KernelT, cl::NullRange, cl::NDRange(GlobalSizeX, GlobalSizeY), cl::NDRange(LocalSizeX, LocalSizeY), 0, &Event);
// 	Event.wait();
// 
// 	// Vertical pass
// 	LocalSizeY = GroupSize;
// 	LocalSizeX = 1;
// 	// Global size must be a multiple of local size
// 	GlobalSizeY = (height + LocalSizeY - 1) / LocalSizeY;
// 	GlobalSizeY *= LocalSizeY;
// 	GlobalSizeX = (width + LocalSizeX - 1) / LocalSizeX;
// 	GlobalSizeX *= LocalSizeX;
// 
// 	Index = 0;
// 	m_KernelV.setArg(Index++, ImTempBuffer2);
// 	m_KernelV.setArg(Index++, OutBuffer);
// 	m_KernelV.setArg(Index++, NewHeight);
// 	m_KernelV.setArg(Index++, NewWidth);
// 	m_KernelV.setArg(Index++, m_Radius);
// 	m_KernelV.setArg(Index++, ByteAlign);
// 
// 	Queue.enqueueNDRangeKernel(m_KernelV, cl::NullRange, cl::NDRange(GlobalSizeY, GlobalSizeX), cl::NDRange(LocalSizeY, LocalSizeX), 0, &Event);
// 	Event.wait();
// 
// 	Queue.enqueueReadBuffer(OutBuffer, true, 0, NewNbPixel * sizeof(unsigned char), (void*)NewOut, 0, &Event);
// 	Event.wait();
// 
// 	Queue.finish();
// 
// 	// Read Sub rect
// 	for (int i = 0; i < height; ++i)
// 	{
// 		for (int j = 0; j < width; ++j)
// 		{
// 			int NewImageIndex = ((i + m_Radius + ByteAlign) * NewWidth) + m_Radius + ByteAlign + j;
// 			int OldImageIndex = i * width + j;
// 			m_ResultData[OldImageIndex] = NewOut[NewImageIndex];
// 		}
// 	}
// 
// 	delete[] NewImageData;
// 	delete[] NewOut;
// 	delete[] TempMask;
// }
// 
// 
