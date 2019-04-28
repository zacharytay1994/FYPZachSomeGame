#pragma once

#include "Surface.h"
#include "Colors.h"
#include "Vec3.h"

#include <string>
#include <vector>

class HeightMap {
public:
	HeightMap(const std::string& image, const int& width, const int& height, const float& minHeight, const float& maxHeight) 
		:
		image(image),
		width(width),
		height(height),
		minHeight(minHeight),
		maxHeight(maxHeight)
	{
		FillHeightMap();
	}
	// fills height/y buffer with values based on the average color channels of pixels at positions on the passed in image scaled to fit the vertex and node grid size
	void FillHeightMap() {
		float unitX = (float)(image.GetWidth() / width);
		float unitY = (float)(image.GetHeight() / height);
		Vecf3 holder;

		for (int y = 0; y < width; y++) {
			for (int x = 0; x < height;  x++) {
				holder = (Vecf3)image.GetPixel((int)(x*unitX), (int)(y*unitY));
				float averageColor = (holder.x + holder.y + holder.z) / 3.0f;
				heightDisplacementGrid.push_back((float)(averageColor*unitHeight));
			}
		}
		
	}
public:
	Surface image;
	const int width;
	const int height;
	const float minHeight;
	const float maxHeight;
	const float unitHeight = (maxHeight - minHeight) / 255.0f;
	std::vector<float> heightDisplacementGrid;
	
};