#pragma once
#include "BaseProcessor.h"
#include <string>

class Sobel : public BaseProcessor
{
private:
	cl::Buffer	filterHor, filterVer;
	cl::Image2D	inputImg, outputImg;
	//图像基本信息
	int			height, width, pitch;
	std::string file;
public:
	Sobel(std::string fileName);
	~Sobel(void);
	//
	virtual void SetArgument();
	virtual void GetArgument();
	void LoadImage2D(std::string& fileName);
	void SaveImage2D(std::string& fileName, unsigned char* buffer);
	//执行
	void RunSobel(std::string kernelSource);
};
