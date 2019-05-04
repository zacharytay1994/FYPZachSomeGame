#pragma once

#include "ProjectileParent.h"

class ProjectileOne : public ProjectileParent{
public:
	ProjectileOne(const Vecf3& loc, const Vecf3& targetLocation) 
		:
		// (projectileSize, projectileLocation, targetLocation, projectileSpeed, projectionType)
		ProjectileParent(0.1f, loc, targetLocation, 12.0f, ProjectionType::low) 
	{}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
	virtual void Draw() override {}
private:
};