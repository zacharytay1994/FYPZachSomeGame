#pragma once

#include "Surface.h"
#include "Colors.h"
#include "Vec2.h"

#include <vector>

class Distortion {
public:
	Distortion() 
		:
		dudvMap("waterDuDv.bmp"),
		normalMap("normalMap.bmp")
	{
		width = dudvMap.GetWidth();
		height = dudvMap.GetHeight();
		NMWidth = normalMap.GetWidth();
		NMHeight = normalMap.GetHeight();
		CalculateNormals();
	}
	void Update(float dt) {
		moveFactor += waveSpeed * dt;
		nmMoveFactor += nmSpeed * dt;
	}
	void SampleDuDv(Color* reflectionIn, Color* refractionIn, const int& bufferWidth, const int& bufferHeight, const float& strength, std::vector<Color>& reflectionOut, std::vector<Color>& refractionOut) {
		Vecf3 distortColor;
		Vecf2 distortVal;
		//std::vector<Vecf3> normalMapDistort;
		int distortY;
		int distortX;
		for (int y = 0; y < bufferHeight; y++) {
			for (int x = 0; x < bufferWidth; x++) {
				int test1 = int(((float)(int(float(x)+moveFactor*2)% bufferWidth) / (float)bufferWidth)*(float)width);
				int test2 = int(((float)(int(float(y)+moveFactor)% bufferHeight) / (float)bufferHeight)*(float)height);
				// sample value from dudv map
				distortColor = Vecf3(dudvMap.GetPixel(test1, test2));
				distortVal = ConvertColorToVecf2(distortColor);
				distortY = std::clamp((int(float(y) + distortVal.y*strength)), 1, bufferHeight-1);
				distortX = std::clamp((int(float(x) + distortVal.x*strength*2)), 1, bufferWidth-1);
				reflectionOut.push_back(reflectionIn[distortY*bufferWidth +distortX]);
				refractionOut.push_back(refractionIn[distortY*bufferWidth +distortX]);
			}
		}
	}
	std::vector<Vecf3> DistortNormalMap(const int& strength) {
		std::vector<Vecf3> outputMap;
		Vecf3 distortColor;
		Vecf2 distortVal;
		int distortX;
		int distortY;
		for (int y = 0; y < NMHeight; y++) {
			for (int x = 0; x < NMWidth; x++) {
				int distortedX = int(((float)(int(float(x) + nmMoveFactor*strength*2) % NMWidth) / (float)NMWidth)*(float)width);
				int distortedY = int(((float)(int(float(y) + nmMoveFactor*strength) % NMHeight) / (float)NMHeight)*(float)height);
				distortColor = (Vecf3)dudvMap.GetPixel(distortedX, distortedY);
				distortVal = ConvertColorToVecf2(distortColor);
				distortY = std::clamp((int(float(y) + distortVal.y*5.0f)), 0, NMHeight - 10);
				distortX = std::clamp((int(float(x) + distortVal.x*5.0f)), 0, NMWidth - 10);
				outputMap.push_back(normals[distortY*NMWidth+distortX]);
			}
		}
		return outputMap;
	}
	Vecf2 ConvertColorToVecf2(const Vecf3 distortColor) {
		return { (distortColor.x/255.0f)*2-1, (distortColor.y/255.0f)*2-1 };
	}
	void CalculateNormals() {
		for (int y = 0; y < normalMap.GetHeight(); y++) {
			for (int x = 0; x < normalMap.GetWidth(); x++) {
				Vecf3 pixelColor = (Vecf3)normalMap.GetPixel(x, y);
				normals.emplace_back(Vecf3((pixelColor.x / 255.0f) * 2 - 1, pixelColor.z / 255.0f, (pixelColor.y / 255.0f) * 2 - 1).GetNormalized());
			}
		}
	}
public:
	std::vector<Vecf3> normals;
	int NMWidth;
	int NMHeight;
private:
	Surface dudvMap;
	Surface normalMap;
	int width;
	int height;
	float moveFactor = 0.0f;
	float waveSpeed = 15.0f;

	float nmMoveFactor = 0.0f;
	float nmSpeed = 5.0f;
};