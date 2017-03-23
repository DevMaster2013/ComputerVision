#include "..\ImageProcessingLibrary\Image.h"
#include <memory.h>

int main()
{
	improc::ImageU8 image("..\\Images\\testimage.jpg");
	
	for (size_t x = 0; x < 50; x++)
	{
		for (size_t y = 0; y < 50; y++)
		{
			image(x, y, 0) = 255;
			image(x, y, 1) = 0;
			image(x, y, 2) = 0;
		}
	}

	image.save("..\\Images\\testimage.png");

	return 0;
}