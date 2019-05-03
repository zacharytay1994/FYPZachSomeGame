#pragma once

#include "ProjectileParent.h"

class ProjectileThree : public ProjectileParent {
public:
	ProjectileThree(const Vecf3& loc, const Vecf3& targetLocation)
		:
		// (projectileSize, projectileLocation, targetLocation, projectileSpeed, projectionType)
		ProjectileParent(0.2f, loc, targetLocation, 8.0f, ProjectionType::lob)
	{}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
	virtual void Draw() override {}
private:
};