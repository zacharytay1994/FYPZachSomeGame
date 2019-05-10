#pragma once

#include "ProjectileParent.h"

class ProjectileTwo : public ProjectileParent {
public:
	ProjectileTwo(const Vecf3& loc, const Vecf3& velocity)
		:
		// (projectileSize, projectileLocation, velocity)
		ProjectileParent(0.2f, loc, velocity)
	{}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
	virtual void Draw() override {}
private:
};