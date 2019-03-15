#include "Surface.h"
#include <assert.h>

Surface::Surface(int width, int height)
	:
	width(width),
	height(height),
	nPixels(new Color[width*height])
{
}

Surface::Surface(const Surface & rhs)
	:
	Surface(rhs.width, rhs.height)
{
	const int size = width * height;
	for (int i = 0; i < size; i++) {
		nPixels[i] = rhs.nPixels[i];
	}
}

Surface & Surface::operator=(const Surface & rhs)
{
	width = rhs.width;
	height = rhs.height;

	delete[] nPixels;
	nPixels = new Color[width*height];
	const int size = width * height;
	for (int i = 0; i < size; i++) {
		nPixels[i] = rhs.nPixels[i];
	}
	// TODO: insert return statement here
	return *this;
}

Surface::~Surface()
{
	delete[] nPixels;
	nPixels = nullptr;
}

void Surface::PutPixel(int x, int y, Color c)
{
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
	nPixels[y * width + x] = c;
}

Color Surface::GetPixel(int x, int y) const
{
	assert(x >= 0);
	assert(x < width);
	assert(y >= 0);
	assert(y < height);
	return nPixels[y * width + x];
}

int Surface::GetWidth()
{
	return width;
}

int Surface::GetHeight()
{
	return height;
}
