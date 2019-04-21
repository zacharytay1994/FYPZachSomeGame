#pragma once

#include "Entity.h"

// mother/base entity
class EntityOne : public Entity { 
public:
	EntityOne(const float& size, const Veci3& loc) 
		:
		Entity(size, loc)
	{
		SetVelocity({ 1.0f, 0.0f, -1.0f });
	}
	EntityOne(const float& size, const Veci2& loc)
		:
		Entity(size, loc)
	{
		SetVelocity({ 1.0f, 0.0f, -1.0f });
	}
	virtual void Update() override {
		// changing location based on velocity
		//Set3DLocation(locationOnBoard3D + (Veci3)velocity);
		// testing simple commands
		if (locationOnBoard3D.x < 0 || locationOnBoard3D.x > 99) {
			velocity.x *= -1;
		}
		if (locationOnBoard3D.z < 0 || locationOnBoard3D.z > 99) {
			velocity.z *= -1;
		}
	}
	virtual void Draw() override {

	}
private:
};