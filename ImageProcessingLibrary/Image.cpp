#include "Image.h"
#include <string.h>
#include <exception>
#include <FreeImage.h>
#include <string>

namespace improc
{
	template<typename PixelType>
	inline Image<PixelType>::Image()
		: _imageWidth(0)
		, _imageHeight(0)
		, _imageChannels(0)
		, _imagePixels(nullptr)
	{
	}

	template<typename PixelType>
	inline Image<PixelType>::Image(const char * fileName)
	{
		load(fileName);
	}

	template<typename PixelType>
	inline Image<PixelType>::Image(size_t imageWidth, size_t imageHeight, size_t imageChannels)
		: _imageWidth(imageWidth)
		, _imageHeight(imageHeight)
		, _imageChannels(imageChannels)
		, _imagePixels(nullptr)
	{
		rsize_t imageSize = _imageWidth * _imageHeight * _imageChannels * sizeof(PixelType);
		_imagePixels = new PixelType[imageSize];
	}

	template<typename PixelType>
	inline Image<PixelType>::Image(size_t imageWidth, size_t imageHeight, size_t imageChannels, PixelType * pixels)
		: _imageWidth(imageWidth)
		, _imageHeight(imageHeight)
		, _imageChannels(imageChannels)
		, _imagePixels(pixels)
	{
	}

	template<typename PixelType>
	inline Image<PixelType>::Image(const Image & src)
		: _imageWidth(src._imageWidth)
		, _imageHeight(src._imageHeight)
		, _imageChannels(src._imageChannels)
		, _imagePixels(nullptr)
	{
		rsize_t imageSize = _imageWidth * _imageHeight * _imageChannels * sizeof(PixelType);
		_imagePixels = new PixelType[imageSize];
		memcpy_s(_imagePixels, imageSize, src._imagePixels, imageSize);
	}

	template<typename PixelType>
	inline Image<PixelType>::Image(Image && src)
		: _imageWidth(src._imageWidth)
		, _imageHeight(src._imageHeight)
		, _imageChannels(src._imageChannels)
		, _imagePixels(src._imagePixels)
	{
		src._imagePixels = nullptr;
	}

	template<typename PixelType>
	inline Image<PixelType>::~Image()
	{
		if (_imagePixels)
			delete[] _imagePixels;
	}

	template<typename PixelType>
	inline Image<PixelType> & Image<PixelType>::operator=(const Image & src)
	{
		if (&src != this)
		{
			_imageWidth = src._imageWidth;
			_imageHeight = src._imageHeight;
			_imageChannels = src._imageChannels;

			rsize_t imageSize = _imageWidth * _imageHeight * _imageChannels * sizeof(PixelType);
			_imagePixels = new PixelType[imageSize];
			memcpy_s(_imagePixels, imageSize, src._imagePixels, imageSize);
		}

		return *this;
	}

	template<typename PixelType>
	inline Image<PixelType> & Image<PixelType>::operator=(Image && src)
	{
		if (&src != this)
		{
			_imageWidth = src._imageWidth;
			_imageHeight = src._imageHeight;
			_imageChannels = src._imageChannels;
			_imagePixels = src._imagePixels;
			src._imagePixels = nullptr;
		}

		return *this;
	}

	template<typename PixelType>
	inline PixelType & Image<PixelType>::operator()(size_t x, size_t y, size_t channel)
	{
		if (_imagePixels != nullptr && x >= 0 && x < _imageWidth && y >= 0 
			&& y < _imageHeight && channel >= 0 && channel < _imageChannels)
		{
			PixelType* row = getRow(y);
			size_t index = x * _imageChannels + channel;
			return row[index];
		}

		throw std::exception("invalid image pixel dimensions");
	}

	// Get the image data type in FreeImage format
	template<typename PixelType>
	FREE_IMAGE_TYPE getImageType()
	{
		// Get the type name
		std::string typeName = typeid(PixelType).name();

		// Get the image type
		FREE_IMAGE_TYPE targetImageType;
		if (typeName == "unsigned char")
			targetImageType = FIT_BITMAP;
		else if (typeName == "unsigned short")
			targetImageType = FIT_UINT16;
		else if (typeName == "unsigned int")
			targetImageType = FIT_UINT32;
		else if (typeName == "char")
			targetImageType = FIT_BITMAP;
		else if (typeName == "short")
			targetImageType = FIT_INT16;
		else if (typeName == "int")
			targetImageType = FIT_INT32;
		else if (typeName == "float")
			targetImageType = FIT_FLOAT;
		else if (typeName == "double")
			targetImageType = FIT_DOUBLE;

		return targetImageType;
	}

	template<typename PixelType>
	inline bool Image<PixelType>::save(const char * fileName)
	{
		if (_imagePixels == nullptr)
			return false;

		// Get the image type
		FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(fileName);
		if (fif == FIF_UNKNOWN)
			return false;

		// Allocate data for image
		FIBITMAP* bitmap = FreeImage_Allocate(_imageWidth, _imageHeight, _imageChannels * sizeof(PixelType) * 8);		
		BYTE* imageBytes = FreeImage_GetBits(bitmap);
		FREE_IMAGE_TYPE targetImageType = getImageType<PixelType>();
		FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(bitmap);

		// Copy the image pixels
		size_t scanLineSize = _imageWidth * _imageChannels * sizeof(PixelType);
		rsize_t imageSize = scanLineSize * _imageHeight;
		for (size_t y = 0; y < _imageHeight; y++)
		{
			BYTE* scanLine = FreeImage_GetScanLine(bitmap, y);
			memcpy_s(scanLine, scanLineSize, _imagePixels + (y * scanLineSize), scanLineSize);
		}
		//memcpy_s(imageBytes, imageSize, _imagePixels, imageSize);

		// Convert from the image type to BITMAP type
		FIBITMAP* convertedImage = nullptr;
		if (imageType == targetImageType)
		{
			convertedImage = bitmap;
		}
		else
		{
			FreeImage_Unload(bitmap);
			convertedImage = FreeImage_ConvertToType(bitmap, targetImageType);
		}
		if (convertedImage == nullptr)
			return false;

		// Save the Image
		if (!FreeImage_Save(fif, convertedImage, fileName))
			return false;
		
		// Unload the image		
		FreeImage_Unload(convertedImage);

		return true;
	}

	template<typename PixelType>
	inline bool Image<PixelType>::load(const char * fileName)
	{
		// Get the image type
		FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename(fileName);
		if (fif == FIF_UNKNOWN)
			return false;

		// Load the Image
		FIBITMAP* bitmap = FreeImage_Load(fif, fileName);
		if (bitmap == nullptr)
			return false;

		// Extract image Info
		_imageWidth = FreeImage_GetWidth(bitmap);
		_imageHeight = FreeImage_GetHeight(bitmap);

		// Get the channels
		FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(bitmap);
		switch (colorType)
		{
		case FIC_RGB:
			_imageChannels = 3;
			break;
		case FIC_PALETTE:
			_imageChannels = 1;
			break;
		case FIC_RGBALPHA:
			_imageChannels = 4;
			break;
		default:
			_imageChannels = 3;
			break;
		}

		// Check the target image type
		FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(bitmap);
		FREE_IMAGE_TYPE targetImageType = getImageType<PixelType>();

		// Convert the image type
		FIBITMAP* convertedImage = nullptr;
		if (imageType == targetImageType)
		{
			convertedImage = bitmap;
		}
		else
		{
			FreeImage_Unload(bitmap);
			convertedImage = FreeImage_ConvertToType(bitmap, targetImageType);
		}			

		if (convertedImage == nullptr)
			return false;

		// Copy the image pixels to the desired image type
		size_t scanLineSize = _imageWidth * _imageChannels * sizeof(PixelType);
		rsize_t imageSize = scanLineSize * _imageHeight;
		_imagePixels = new PixelType[imageSize];
		for (size_t y = 0; y < _imageHeight; y++)
		{
			BYTE* scanLine = FreeImage_GetScanLine(convertedImage, y);
			memcpy_s(_imagePixels + (y * scanLineSize), scanLineSize, scanLine, scanLineSize);
		}

		// Unload the image		
		FreeImage_Unload(convertedImage);

		return true;
	}

	template<typename PixelType>
	inline PixelType * Image<PixelType>::getRow(size_t row)
	{
		if (_imagePixels != nullptr && row >= 0 && row < _imageHeight)
		{
			size_t index = (_imageHeight - row - 1) * _imageWidth * _imageChannels;
			return (_imagePixels + index);
		}

		return nullptr;
	}

	template<typename PixelType>
	size_t Image<PixelType>::getWidth()
	{
		return _imageWidth;
	}

	template<typename PixelType>
	size_t Image<PixelType>::getHeight()
	{
		return _imageHeight;
	}

	template<typename PixelType>
	size_t Image<PixelType>::getChannels()
	{
		return _imageChannels;
	}

	template class IMPROC_API Image<unsigned char>;
	template class IMPROC_API Image<unsigned short>;
	template class IMPROC_API Image<unsigned int>;
	template class IMPROC_API Image<char>;
	template class IMPROC_API Image<short>;
	template class IMPROC_API Image<int>;
	template class IMPROC_API Image<float>;
	template class IMPROC_API Image<double>;
}