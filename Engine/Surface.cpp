#include "Surface.h"
#include "ChiliWin.h"
#include <assert.h>
#include <fstream>

Surface::Surface(int width, int height)
	:
	width(width),
	height(height),
	nPixels(new Color[width*height])
{
}

Surface::Surface(const std::string & filename)
{
	// Open file
	std::ifstream file(filename, std::ios::binary);
	// Bitmap file header struct
	BITMAPFILEHEADER bitmapfileheader;
	// Bitmap info header struct
	BITMAPINFOHEADER bitmapinfoheader;

	// Read file into bitmapfileheader
	file.read(reinterpret_cast<char*>(&bitmapfileheader), sizeof(bitmapfileheader));

	file.read(reinterpret_cast<char*>(&bitmapinfoheader), sizeof(bitmapinfoheader));

	assert(bitmapinfoheader.biBitCount == 24);
	assert(bitmapinfoheader.biCompression == BI_RGB);

	width = bitmapinfoheader.biWidth;
	height = bitmapinfoheader.biHeight;

	nPixels = new Color[width*height];
	// finding padding size
	const int padding = (4 - ((width * 3) % 4)) % 4;
	// seeking to start of texture
	file.seekg(bitmapfileheader.bfOffBits);
	// flipping the image because bitmap stores it upside down
	for (int y = height-1; y >= 0; y--) {
		for (int x = 0; x < width; x++) {
			//nPixels[y*width + x] = file.get() << file.get() << file.get();
			PutPixel(x, y, Color(file.get(), file.get(), file.get()));
		}
		file.seekg(padding, std::ios::cur);
	}

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

void Surface::SetWidth(const int & widthIn)
{
	width = widthIn;
}

void Surface::SetHeight(const int & heightIn)
{
	height = heightIn;
}

void Surface::Reset(int widthIn, int heightIn)
{
	width = widthIn;
	height = heightIn;
	delete[] nPixels;
	nPixels = new Color[width*height];
}
