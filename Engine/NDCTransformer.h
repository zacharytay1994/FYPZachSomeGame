#pragma once

#include "Vec3.h"
#include "Graphics.h"

template <class T>
class NDCTransformer {
public:
	NDCTransformer()
		:
		horizontalF(float(Graphics::ScreenWidth) / 2.0f),
		verticalF(float(Graphics::ScreenHeight) / 2.0f)
	{}
	T& Transform(T& v) const {
		// Perspective projection
		const float wInv = 1.0f / v.pos.w;
		// NDC to screen space
		// z divide the entire vertex
		v = v * wInv;
		// transform x and y into screenspace
		v.pos.x = (v.pos.x + 1.0f) * horizontalF;
		v.pos.y = (-v.pos.y + 1.0f) * verticalF;

		/*v.normal.x = (v.normal.x + 1.0f) * horizontalF;
		v.normal.y = (-v.normal.y + 1.0f) * verticalF;*/
		// store z inverse for calculation purposes later on
		v.pos.w = wInv;
		return v;
	}
	T& GetTransformed(const T& v) const {
		return Transform(T(v));
	}
private:
	float horizontalF;
	float verticalF;
};