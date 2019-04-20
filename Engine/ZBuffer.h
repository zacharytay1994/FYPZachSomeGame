#pragma once

#include <assert.h>
#include <memory>

class ZBuffer {
public:
	ZBuffer(const int& width, const int& height) 
		:
		width(width),
		height(height),
		zBuffer(new float[width * height]),
		pointBuffer(new Vecf2[(width * height)])
	{}
	// reset buffer data
	void Clear() {
		const int size = width * height;
		for (int i = 0; i < size; i++) {
			zBuffer[i] = std::numeric_limits<float>::infinity();
			pointBuffer[i] = Vecf2(0.0f, 0.0f);
		}
	}
	// get at position
	float& At(int x, int y) {
		assert(x >= 0);
		assert(x < width);
		assert(y >= 0);
		assert(y < height);
		return zBuffer[y * width + x];
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
public:
	int width;
	int height;
	std::unique_ptr<Vecf2[]> pointBuffer;
private:
	std::unique_ptr<float[]> zBuffer;
};