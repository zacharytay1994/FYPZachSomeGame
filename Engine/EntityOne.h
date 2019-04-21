#pragma once

#include "Entity.h"

// mother/base entity
class EntityOne : public Entity { 
public:
	EntityOne(const float& size, const Veci2& loc) 
		:
		Entity(size, loc)
	{
		SetVelocity({ 0.5f, 0.0f, -0.5f });
	}
	virtual void Update() override {

	}
	virtual void Draw() override {

	}
private:
};