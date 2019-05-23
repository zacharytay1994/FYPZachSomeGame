#include "Steering.h"

Vecf3 Steering::Seek(Vecf3 & currentVelocity, const Vecf3 & currentLocation, const Vecf3 & targetDestination, const float& speed, const float & turningForce)
{
	// calculate desired velocity
	Vecf3 desiredVelocity = (targetDestination - currentLocation).GetNormalized() * speed;
	// get and return steering velocity
	Vecf3 steeringForce = (desiredVelocity - currentVelocity).GetNormalized();
	// limit the steering force
	return currentVelocity = currentVelocity + steeringForce * turningForce;
}