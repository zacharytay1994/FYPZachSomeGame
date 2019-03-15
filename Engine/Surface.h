#pragma once

#include "Colors.h"

class Surface {
public:
	Surface(int width, int height);
	Surface(const Surface&);
	Surface& operator=(const Surface& rhs);
	~Surface();
	void PutPixel(int x, int y, Color c);
	Color GetPixel(int x, int y) const;
	int GetWidth();
	int GetHeight();
private:
	int width;
	int height;
	Color* nPixels = nullptr;
};
