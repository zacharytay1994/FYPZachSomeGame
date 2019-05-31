#pragma once

#include "Surface.h"
#include "Colors.h"
#include "Vec2.h"

class Distortion {
public:
	Distortion() 
		:
		dudvMap("dudvmap1.bmp")
	{}
	Vecf2 SampleDuDv(Color* colorBuffer) {

	}
private:
	Surface dudvMap;
	int width = dudvMap.GetWidth();
	int height = dudvMap.GetHeight();
};