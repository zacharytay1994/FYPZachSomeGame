#pragma once

#include "ProjectileParent.h"

#include <sstream>
#include <string>

class ProjectileOne : public ProjectileParent{
public:
	ProjectileOne(const Vecf3& loc, const Vecf3& targetLocation) 
		:
		ProjectileParent(0.5f, loc, targetLocation, 12.0f, ProjectionType::low) // (projectileSize, projectileLocation, targetLocation, projectileSpeed)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		// Update location with velocity
		spawnLocationOffset += velocity * dt;
		// change velocity due to gravitational acceleration
		velocity += gravitationalAcc * dt;
	}
	virtual void Draw() override {

	}
private:
};