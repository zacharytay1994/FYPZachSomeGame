#pragma once

#include "Vec3.h" 
#include "MathStuff.h"

#include <cmath>

class Physics {
public:
	// returns projection angle knowing magnitude and range
	static bool GetProjectionAngle(const Vecf3& startPos, const Vecf3& endPos, const float& magnitude, float& theta) {
		// difference in the respective axes
		const float xDifference = abs(endPos.x - startPos.x);
		const float zDifference = abs(endPos.z - startPos.z);
		// respective displacements on their planes
		const float horizontalDisplacement = abs(sqrt(sq(xDifference) + sq(zDifference)));
		const float verticalDisplacement = endPos.y - startPos.y;
		// check to prevent values lower than -1 or greater than 1 to be passed into arcsine
		float toBeASinVal =  2.0f * (-horizontalDisplacement * -9.81f) / (2 * sq(magnitude));
		if ((toBeASinVal) > 1.0f || (toBeASinVal) < -1.0f) {
			return false;
		}
		else {
			float theta = std::asin(toBeASinVal) / 2;
			theta = theta;
			return true;
		}
	}
	static bool GetProjectionAngleElevated(const Vecf3& startPos, const Vecf3& endPos, const float& magnitude, float& theta, const int& typeOfThrow) {
		// difference in the the respective axes
		const float xDifference = endPos.x - startPos.x;
		const float zDifference = endPos.z - startPos.z;
		// displacement on the horizontal plane
		const float horizontalDisplacement = sqrt(sq(xDifference) + sq(zDifference));
		// displacement on the vertical plane
		const float verticalDisplacement = endPos.y - startPos.y;
		// pre calculated constant representing (0.5 * gravitationalAcceleration * horizontalDisplacement^2 / magnitude^2)
		const float k = (0.5f * -9.81f * sq(horizontalDisplacement)) / (sq(magnitude));
		// pre calculated constant using constant above
		const float kkv = (k * (k - verticalDisplacement));
		// checking if quadratic equation results in a result (b^2 - 4ac) > || < 0, should probably also check == 0 but is unlikely
		// due to rounding errors
		const float bsm4ac = sq(horizontalDisplacement) - 4 * kkv;
		if (bsm4ac > 0) {
			// first root
			const float passOne = (-horizontalDisplacement + sqrt(bsm4ac)) / (2 * k);
			// second root
			const float passTwo = (-horizontalDisplacement - sqrt(bsm4ac)) / (2 * k);
			// calculating the respective thetas
			const float thetaOne = std::atan(passOne);
			const float thetaTwo = std::atan(passTwo);
			// getting the smaller angle out of the two
			// if a low throw
			if (typeOfThrow == 0) {
				// return the smaller number
				theta = thetaOne < thetaTwo ? thetaOne : thetaTwo;
				return true;
			}
			// else its a lob
			else if (typeOfThrow == 1) {
				// return the larger number
				theta = thetaOne > thetaTwo ? thetaOne : thetaTwo;
				return true;
			}
		}
		return false;
	}
private:
};