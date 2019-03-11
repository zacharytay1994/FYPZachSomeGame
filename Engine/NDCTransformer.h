#pragma once

#include "Vec2.h"
#include "Graphics.h"

class NDCTransformer {
public:
	NDCTransformer()
		:
		horizontalF(float(Graphics::ScreenWidth) / 2.0f),
		verticalF(float(Graphics::ScreenHeight) / 2.0f)
	{}
	Vecf2& Transform(Vecf2& v) const {
		v.x = (v.x + 1.0f) * horizontalF;
		v.y = (-v.y + 1.0f) * verticalF;
		return v;
	}
	Vecf2& GetTransformed(const Vecf2& v) const {
		return Transform(Vecf2(v));
	}
private:
	float horizontalF;
	float verticalF;
};