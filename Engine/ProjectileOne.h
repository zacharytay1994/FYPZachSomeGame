#pragma once

#include "ProjectileParent.h"

class ProjectileOne : public ProjectileParent{
public:
	ProjectileOne(const Vecf3& loc) 
		:
		ProjectileParent(0.5f, loc) // (size, location)
	{}
	virtual void Update() override {
		spawnLocationOffset += velocity;
	}
	virtual void Draw() override {

	}
private:
	const Vecf3 velocity = { 0.01f, 0.0f, 0.0f };
};