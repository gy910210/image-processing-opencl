#include "StdAfx.h"
#include "BaseProcessor.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

BaseProcessor::BaseProcessor()
{
	//基类初始化
	BOOL result = CheckError(CL_SUCCESS, "Base Construction...");
	//获得platform信息
	errorNum = cl::Platform::get(&platforms);
	result = CheckError(errorNum, "Getting Platforms...");
	//获得平台信息和设备信息
	std::string platformName;
	std::string deviceType;
	std::cout	<<	"Number of Platforms Available: "	
				<<	platforms.size()
				<<	std::endl;
	//无可用设备，退出
	if (platforms.size() <= 0)
	{
		std::cout	<<	"No Available Platforms..."
					<<	std::endl;
		getchar();
		exit(-1) ;
	}
	//获得平台名称
	platforms[0].getInfo(CL_PLATFORM_NAME, &platformName);
	std::cout	<<	"Platform Used: "
				<<	platformName
				<<	std::endl;
	//获得设备信息
	errorNum = platforms[0].getDevices(CL_DEVICE_TYPE_ALL, &devices);
	result = CheckError(errorNum, "Getting Devices...");
	std::cout	<<	"Number of Devices Available: "
				<<	devices.size()
				<<	std::endl;
	cl_int num = 0;
	devices[0].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &num);
	if (devices.size() <= 0)
	{
		std::cout	<<	"No Available Devices..."
			<<	std::endl;
		getchar();
		exit(-1) ;
	}
	//获得设备名称和CU的最大数量
	errorNum = devices[0].getInfo(CL_DEVICE_NAME, &deviceType);
	std::cout	<<	"Devices Used: "
				<<	deviceType
				<<", "<<"Compute units: "
				<<num
				<<	std::endl;
	//获得上下文信息
	context = cl::Context(devices, NULL, NULL, NULL, &errorNum);
	result = CheckError(errorNum, "Getting Context Information...");
	//获得任务队列
	commandQueue = cl::CommandQueue(context, devices[0], NULL, &errorNum);
	result = CheckError(errorNum, "Getting Command Queue...");
}


BaseProcessor::~BaseProcessor()
{
}

BOOL BaseProcessor::CheckError(cl_int error, const char* stepDesp)
{
	if (error != CL_SUCCESS)
	{
		std::cout	<<	"Error happened when ["
					<<	stepDesp
					<<	"] Error Code = "
					<<	error
					<<	std::endl;
		getchar();
		exit(-1);
		return FALSE;
	}

	std::cout	<<	stepDesp
				<<	"SUCCESS!"
				<<	std::endl;

	return TRUE;
}

std::string BaseProcessor::LoadKernelSourceFromFile(std::string fileName, cl_int& error)
{
	std::ifstream in(fileName, std::ios_base::binary);
	if(!in.good()) 
	{
		error = -1;
		return "";
	}
	//获得文件程度
	in.seekg(0, std::ios_base::end);
	size_t length = in.tellg();
	in.seekg(0, std::ios_base::beg);
	//获得代码
	char* data = new char[length+1];
	in.read(&data[0], length);
	data[length] = 0;
	//转化为string
	std::string kernelSource(data);
	delete[] data;
	error = CL_SUCCESS;
	return kernelSource;
}


void BaseProcessor::LoadProgram(std::string kernelSource)
{
	CheckError(CL_SUCCESS, "Loading Program...");
	//从二进制代码代开内核
	char* path = new char[kernelSource.size() + 8];
	memcpy(path, kernelSource.c_str(), kernelSource.size());
	memcpy(&path[kernelSource.size()], ".cl.bin", 7);
	path[kernelSource.size()+7] = 0;
	FILE* fp = fopen(path, "rb");
	delete[] path;
	if (fp != NULL)
	//if(0)
	{//存在bin文件
		size_t binarySize;
		//获得文件尾位置
		fseek(fp,0,SEEK_END);
		binarySize = ftell(fp);
		rewind(fp);
		unsigned char* programBinary = new unsigned char[binarySize];
		fread(programBinary, 1, binarySize, fp);
		fclose(fp);
		//构建Binaries内核对象
		cl::Program::Binaries binaries(1, make_pair(programBinary, binarySize));
		//从内核对象创建Program
		program = cl::Program(context, devices, binaries, NULL, &errorNum);
		CheckError(errorNum, "Loading From Binary Source...");
		//build程序
		errorNum = program.build(devices);
		delete[] programBinary;
		CheckError(errorNum, "Building Program from Binary...");	
	}
	else
	{//否则从源代码中读取
		std::string kernelSourceString = LoadKernelSourceFromFile(kernelSource.append(".cl"), errorNum);
		CheckError(errorNum, "Loading From Text Source...");
		cl::Program::Sources source(1, make_pair(kernelSourceString.c_str(), kernelSourceString.size()));
		//从源代码创建Program
		program = cl::Program(context, source, &errorNum);
		//build程序
		errorNum = program.build(devices);
		//BOOL result = CheckError(errorNum, "Building Program from Source...");	
		//if (!result)
		if(errorNum!=CL_SUCCESS)
		{
			std::cout << "================================================================" << std::endl;
			std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
			std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
			std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;

			/*char buildLog[16384];
			program.getBuildInfo(devices[0], CL_PROGRAM_BUILD_LOG, buildLog);
			std::cout	<<	"Build Error Log:\n"
						<<	buildLog;*/

			getchar( );
			exit(-1);
		}
		//将源码保存为binary版本，方便下次调用
		cl_int num;
		errorNum = program.getInfo(CL_PROGRAM_NUM_DEVICES, &num);
		cl_device_id* deviceList = new cl_device_id[num];
		errorNum |= program.getInfo(CL_PROGRAM_DEVICES, deviceList);
		size_t* programBinarySizes = new size_t[num];
		errorNum |= program.getInfo(CL_PROGRAM_BINARY_SIZES, programBinarySizes);
		unsigned char ** programBinaries = new unsigned char *[num];
		for (cl_uint i = 0; i < num; i++)
			programBinaries[i] = new unsigned char[programBinarySizes[i]];
		errorNum |= program.getInfo(CL_PROGRAM_BINARIES, programBinaries);
		FILE* fp = fopen(kernelSource.append(".bin").c_str(), "wb");
		if (fp != NULL)
		{
			fwrite(programBinaries[0],1,programBinarySizes[0],fp);
			fclose(fp);
			for (cl_uint i = 0; i < num; i++)
			{
				delete[] programBinaries[i];
				programBinaries[i] = NULL;
			}
			delete[] programBinaries;
			programBinaries = NULL;
			CheckError(errorNum, "Saving Text Source TO Binary Source...");
		}
	}
}

void BaseProcessor::CreateKernel(const std::string kernelName)
{
	//创建内核
	kernel = cl::Kernel(program, kernelName.data(), &errorNum);
	CheckError(errorNum, "Creating Kernel...");
	SetArgument();
}

void BaseProcessor::RunKernel(cl::NDRange& offset, cl::NDRange& global, cl::NDRange& local)
{
	//执行内核
	errorNum = commandQueue.enqueueNDRangeKernel(kernel, offset, global, local);
	errorNum |= commandQueue.finish();
	CheckError(errorNum, "Running Kernel...");
}