#pragma once

#include "Entity.h"

class ProjectileParent : public Entity {
public:
	ProjectileParent(const float& size, const Vecf3& loc)
		:
		Entity(size, loc)
	{}
	virtual void Update() = 0;
	virtual void Draw() = 0;
private:
};