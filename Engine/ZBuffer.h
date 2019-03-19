#pragma once

#include <assert.h>
#include <memory>

class ZBuffer {
public:
	ZBuffer(const int& width, const int& height) 
		:
		width(width),
		height(height),
		zBuffer(new float[width * height])
	{}
	// reset buffer data
	void Clear() {
		const int size = width * height;
		for (int i = 0; i < size; i++) {
			zBuffer[i] = std::numeric_limits<float>::infinity();
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
			return true;
		}
		return false;
	}
private:
	int width;
	int height;
	std::unique_ptr<float[]> zBuffer;
};