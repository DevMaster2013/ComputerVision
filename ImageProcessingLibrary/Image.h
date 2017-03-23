#pragma once

#include "DLLDefinitions.h"

namespace improc
{
	template<typename PixelType>
	class IMPROC_API Image
	{
	private:
		size_t _imageWidth;
		size_t _imageHeight;
		size_t _imageChannels;
		PixelType* _imagePixels;

	public:
		Image();
		Image(const char* fileName);
		Image(size_t imageWidth, size_t imageHeight, size_t imageChannels);
		Image(size_t imageWidth, size_t imageHeight, size_t imageChannels, PixelType* pixels);
		Image(const Image& src);
		Image(Image&& src);
		~Image();

	public:
		Image& operator=(const Image& src);
		Image& operator=(Image&& src);

	public:
		PixelType& operator()(size_t x, size_t y, size_t channel = 0);
		
	public:
		bool save(const char* fileName);
		bool load(const char* fileName);

	public:
		PixelType* getRow(size_t row);

	public:
		size_t getWidth();
		size_t getHeight();
		size_t getChannels();
	};

	using ImageU8	=  Image<unsigned char>;
	using ImageU16	=  Image<unsigned short>;
	using ImageU32	=  Image<unsigned int>;
	using Image8	=  Image<char>;
	using Image16	=  Image<short>;
	using Image32	=  Image<int>;
	using ImageF	=  Image<float>;
	using ImageD	=  Image<double>;
}