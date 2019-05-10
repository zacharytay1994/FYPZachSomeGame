#pragma once

#include "ProjectileParent.h"

class ProjectileThree : public ProjectileParent {
public:
	ProjectileThree(const Vecf3& loc, const Vecf3& velocity)
		:
		// (projectileSize, projectileLocation, targetLocation, projectileSpeed, projectionType)
		ProjectileParent(0.3f, loc, velocity)
	{}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
	virtual void Draw() override {}
private:
};