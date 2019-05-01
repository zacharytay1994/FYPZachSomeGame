#pragma once

#include "Vec3.h" 
#include "MathStuff.h"

#include <cmath>

class Physics {
public:
	// returns projection angle knowing magnitude and range
	static bool GetProjectionAngle(const Vecf3& startPos, const Vecf3& endPos, const float& magnitude, float& projectionAngleCos, float& projectionAngleSin) {
		const float xDifference = abs(endPos.x - startPos.x);
		const float zDifference = abs(endPos.z - startPos.z);
		const float horizontalDisplacement = abs(sqrt(sq(xDifference) + sq(zDifference)));
		const float verticalDisplacement = endPos.y - startPos.y;
		/*float testVal1 = (range / magnitude);
		float testVal2 = (sqrt(1 / (0.5f*9.81f)));*/
		//float testVal1 = (verticalDisplacement / magnitude) / (sqrt(sq(1 / (0.5f*9.81f)) + 1));
		//if (testVal1 > 1.0f) {
		//	return false;
		//}
		//else {
		//	float val1 = std::asin(testVal1);
		//	float val2 = std::atan(0.5f*9.81f);
		//	float projectionAngle = val1 + val2;
		//	/*if (projectionAngle > (PI / 2.0f)) {
		//		projectionAngleCos = projectionAngle - (PI / 2);
		//	}
		//	else {*/
		//	projectionAngleCos = projectionAngle;
		//	//}
		//	projectionAngleSin = projectionAngle;
		//	return true;
		//}
		float testVal = (-horizontalDisplacement * -9.81f) / (2 * sq(magnitude));
		if ((2 * testVal) > 1.0f || (2*testVal) < -1.0f) {
			return false;
		}
		else {
			float theta = std::asin(2 * testVal) / 2;
			projectionAngleCos = theta;
			projectionAngleSin = theta;
			return true;
		}
	}
private:
	const float gravitationalAcceleration = 9.81f;
};