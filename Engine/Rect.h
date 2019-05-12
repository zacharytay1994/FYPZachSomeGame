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
	Rect(const Rect& rect) 
		:
		Rect(rect.rectCenter, rect.width, rect.height)
	{}
	bool IsOverlap(const Vecf2& position) {
		return position.x < rightBound &&
			position.x > leftBound &&
			position.y < upperBound &&
			position.y > lowerBound;
	}
	bool IsOverlapRect(const Rect& rectIn) {
		return !(rightBound < rectIn.leftBound ||
			leftBound > rectIn.rightBound ||
			upperBound < rectIn.lowerBound ||
			lowerBound > rectIn.upperBound);
	}
public:
	Vecf2 rectCenter;
	float width;
	float height;
	float leftBound = rectCenter.x - width / 2.0f;
	float rightBound = rectCenter.x + width / 2.0f;
	float upperBound = rectCenter.y + height / 2.0f;
	float lowerBound = rectCenter.y - height / 2.0f;
};
