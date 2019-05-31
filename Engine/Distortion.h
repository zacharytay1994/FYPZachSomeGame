#pragma once

#include "Surface.h"
#include "Colors.h"
#include "Vec2.h"

#include <vector>

class Distortion {
public:
	Distortion() 
		:
		dudvMap("dudvmap2.bmp")
	{
		width = dudvMap.GetWidth();
		height = dudvMap.GetHeight();
	}
	void Update(float dt) {
		moveFactor += waveSpeed * dt;
	}
	void SampleDuDv(Color* reflectionIn, Color* refractionIn, const int& bufferWidth, const int& bufferHeight, const float& strength, std::vector<Color>& reflectionOut, std::vector<Color>& refractionOut) {
		Vecf3 distortColor;
		Vecf2 distortVal;
		int distortY;
		int distortX;
		Color* tempHolder = new Color[bufferWidth*bufferHeight];
		for (int y = 0; y < bufferHeight; y++) {
			for (int x = 0; x < bufferWidth; x++) {
				int test1 = int(((float)(int(float(x)+moveFactor)%bufferWidth) / (float)bufferWidth)*(float)width);
				int test2 = int((float(y) / (float)bufferHeight)*(float)height);
				// sample value from dudv map
				distortColor = Vecf3(dudvMap.GetPixel(test1, test2));
				distortVal = ConvertColorToVecf2(distortColor);
				distortY = std::clamp((int(float(y) + distortVal.y*strength)), 0, bufferHeight-10);
				distortX = std::clamp((int(float(x) + distortVal.x*strength*2)), 0, bufferWidth-10);
				reflectionOut.push_back(reflectionIn[distortY*bufferWidth+distortX]);
				refractionOut.push_back(refractionIn[distortY*bufferWidth+distortX]);
			}
			//((y + int(distortVal.y*strength)) % bufferHeight)*bufferWidth + ((x + int(distortVal.x*strength)) % bufferWidth)
		}
	}
	Vecf2 ConvertColorToVecf2(const Vecf3 distortColor) {
		//Vecf3 colorRange = Vecf3(distortColor);
		return { (distortColor.x/255.0f)*2-1, (distortColor.y/255.0f)*2-1 };
	}
private:
	Surface dudvMap;
	int width;
	int height;
	float moveFactor = 0.0f;
	float waveSpeed = 100.0f;
};