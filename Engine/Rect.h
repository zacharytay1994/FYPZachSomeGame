#pragma once

#include "Vec2.h"

class Rect {
public:
	Rect(const Vecf2& rectcenter, const float& width, const float& height) 
		:
		rectCenter(rectcenter),
		width(width),
		height(height)
	{}
	Rect(const Vecf2& topleft, const Vecf2& topright, const Vecf2& bottomleft, const Vecf2& bottomright) 
		:
		Rect(Vecf2(topleft.x + (topright.x - topleft.x)/2, topleft.y + (bottomleft.y - topleft.y)/2), topright.x - topleft.x, bottomleft.y - topleft.y)
	{}
	bool IsOverlap(const Vecf2& position) {
		return position.x < (rectCenter.x + width / 2) &&
			position.x >(rectCenter.x - width / 2) &&
			position.y < (rectCenter.y + height / 2) &&
			position.y >(rectCenter.y - height / 2);
	}
public:
	Vecf2 rectCenter;
	float width;
	float height;
};
