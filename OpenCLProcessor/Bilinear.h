#pragma once
#include "BaseProcessor.h"
#include <string>
#include "FreeImage.h"

class Bilinear :public BaseProcessor
{
	public:
		Bilinear(std::string fileName);
		~Bilinear( );
		//
		virtual void SetArgument();
		virtual void GetArgument();
		void LoadImage2D(std::string& fileName);
		void SaveImage2D(std::string& fileName, unsigned char* buffer);
		//Ö´ÐÐ
		void RunBilinear(std::string kernelSource);

	private:
		cl::Image2D	inputImg, outputImg;
		std::string		file;
		int				height, width, pitch, t_height, t_width, t_pitch;
		float				size;
		FIBITMAP		*srcImage, *desImage; 
};