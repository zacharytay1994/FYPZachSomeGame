#pragma once

#include "Colors.h"
#include <string>

class Surface {
public:
	Surface(int width, int height);
	Surface(const std::string& filename);
	Surface(const Surface&);
	Surface& operator=(const Surface& rhs);
	~Surface();
	void PutPixel(int x, int y, Color c);
	Color GetPixel(int x, int y) const;
	int GetWidth();
	int GetHeight();
	void SetWidth(const int& widthIn);
	void SetHeight(const int& heightIn);
	void Reset(int widthIn, int heightIn);
private:
	int width;
	int height;
	Color* nPixels = nullptr;
};
