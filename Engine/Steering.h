#pragma once

#include "Vec3.h"

class Steering {
public:
	// returns a velocity to seek to destination position, seeking velocity = desired velocity - current velocity
	static Vecf3 Seek(Vecf3& currentVelocity, const Vecf3& currentLocation, const Vecf3& targetDestination, const float& speed, const float& turningForce);
private:
	
};