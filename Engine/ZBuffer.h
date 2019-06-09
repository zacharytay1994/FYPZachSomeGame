#pragma once

#include "Colors.h"

#include <assert.h>
#include <memory>

class ZBuffer {
public:
	int reflectionBufferWidth = 96;
	int reflectionBufferHeight = 54;
public:
	ZBuffer(const int& width, const int& height) 
		:
		width(width),
		height(height),
		zBuffer(new float[width * height]),
		reflectionZBuffer(new float[width * height]),
		refractionZBuffer(new float[width * height]),
		pointBuffer(new Vecf2[(width * height)]),
		/*reflectionBuffer(new Color[reflectionBufferWidth*reflectionBufferHeight]),
		refractionBuffer(new Color[reflectionBufferWidth*reflectionBufferHeight])*/
		reflectionBuffer(new Color[width * height]),
		refractionBuffer(new Color[width * height])
	{}
	// reset buffer data
	void Clear() {
		const int size = width * height;
		for (int i = 0; i < size; i++) {
			zBuffer[i] = std::numeric_limits<float>::infinity();
			reflectionZBuffer[i] = std::numeric_limits<float>::infinity();
			refractionZBuffer[i] = std::numeric_limits<float>::infinity();
			pointBuffer[i] = Vecf2(0.0f, 0.0f);
			reflectionBuffer[i] = Colors::White;
			refractionBuffer[i] = Colors::White;
		}
		/*int size2 = reflectionBufferWidth * reflectionBufferHeight;
		for (int i = 0; i < size2; i++) {
			reflectionBuffer[i] = Colors::White;
			refractionBuffer[i] = Colors::Black;
		}*/
	}
	// get at position
	float& At(int x, int y) {
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);
		return zBuffer[y * width + x];
	}
	float& AtReflect(int x, int y) {
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);
		return reflectionZBuffer[y * width + (width-x)];
	}
	float& AtRefract(int x, int y) {
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);
		return refractionZBuffer[y * width + x];
	}
	Color& ColorAt(int x, int y) {
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);
		return reflectionBuffer[y*width+x];
	}
	// removes const from ZBuffer and reads data
	const float& At(int x, int y) const {
		return const_cast<ZBuffer*>(this)->At(x, y);
	}
	// test and set points in zBuffer
	bool TestAndSet(int x, int y, float depth) {
		float& depthAtPoint = At(x, y);
		if (depth < depthAtPoint) {
			depthAtPoint = depth;
			pointBuffer[y*width + x] = Vecf2((float)x, (float)y);
			return true;
		}
		return false;
	}
	// test and set with z
	bool TestAndSetZ(int x, int y, float depth, const Vecf2& texpos) {
		float& depthAtPoint = At(x, y);
		if (depth < depthAtPoint) {
			depthAtPoint = depth;
			pointBuffer[y*width + x] = texpos;
			return true;
		}
		return false;
	}
	// test and set with z for reflection
	bool TestAndSetZReflect(int x, int y, float depth) {
		float& depthAtPoint = AtReflect(x, y);
		if (depth < depthAtPoint) {
			depthAtPoint = depth;
			return true;
		}
		return false;
	}
	bool TestAndSetColor(int x, int y, float depth) {
		float& depthAtPoint = At(x, y);
		if (depth < depthAtPoint) {
			depthAtPoint = depth;
			return true;
		}
		return false;
	}
	bool FillReflectionBuffer(int x, int y, float depth, Color color, const Vecf3& clipCoord);
	bool FillRefractionBuffer(int x, int y, float depth, Color color) {
		float& depthAtPoint = AtRefract(x, y);
		if (depth < depthAtPoint) {
			depthAtPoint = depth;
			refractionBuffer[y*width+x] = color;
			return true;
		}
		return false;
	}
public:
	int width;
	int height;
	float ratioValY = float(reflectionBufferHeight / height);
	float ratioValX = float(reflectionBufferWidth / width);
	std::unique_ptr<Vecf2[]> pointBuffer;
	// for water sampling
	std::unique_ptr<Color[]> reflectionBuffer;
	std::unique_ptr<Color[]> refractionBuffer;
private:
	std::unique_ptr<float[]> zBuffer;
	std::unique_ptr<float[]> reflectionZBuffer;
	std::unique_ptr<float[]> refractionZBuffer;
};