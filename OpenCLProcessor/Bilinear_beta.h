#pragma once
#include "BaseProcessor.h"
#include <string>
#include "FreeImage.h"

class Bilinear_beta :public BaseProcessor
{
public:
	Bilinear_beta(std::string fileName);
	~Bilinear_beta( );
	//
	virtual void SetArgument();
	virtual void GetArgument();
	void LoadImage2D(std::string& fileName);
	void SaveImage2D(std::string& fileName);
	//Ö´ÐÐ
	void RunBilinear(std::string kernelSource);

private:
	//cl::Image2D	inputImg, outputImg;
	cl::Buffer		inputImg, outputImg;
	BYTE				*pSrc, *pDes;
	std::string		file;
	int				height, width, pitch, t_height, t_width, t_pitch;
	float				size;
	FIBITMAP		*srcImage, *desImage; 
};