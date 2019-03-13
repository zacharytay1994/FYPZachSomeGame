#pragma once

#include "Vec3.h"
#include "Graphics.h"

class NDCTransformer {
public:
	NDCTransformer()
		:
		horizontalF(float(Graphics::ScreenWidth) / 2.0f),
		verticalF(float(Graphics::ScreenHeight) / 2.0f)
	{}
	Vecf3& Transform(Vecf3& v) const {
		// Perspective projection
		const float zInv = 1.0f / v.z;
		// NDC to screen space
		v.x = (v.x * zInv + 1.0f) * horizontalF;
		v.y = (-v.y * zInv + 1.0f) * verticalF;
		return v;
	}
	Vecf2& GetTransformed(const Vecf3& v) const {
		return Transform(Vecf3(v));
	}
private:
	float horizontalF;
	float verticalF;
};