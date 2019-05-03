#pragma once

#include "ProjectileParent.h"

class ProjectileTwo : public ProjectileParent {
public:
	ProjectileTwo(const Vecf3& loc, const Vecf3& targetLocation)
		:
		// (projectileSize, projectileLocation, targetLocation, projectileSpeed, projectionType)
		ProjectileParent(0.3f, loc, targetLocation, 5.0f, ProjectionType::lob)
	{}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
	virtual void Draw() override {}
private:
};