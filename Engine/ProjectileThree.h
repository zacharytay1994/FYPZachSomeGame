#pragma once

#include "ProjectileParent.h"

class ProjectileThree : public ProjectileParent {
public:
	ProjectileThree(const Vecf3& loc, const Vecf3& velocity, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		// (projectileSize, projectileLocation, targetLocation, projectileSpeed, projectionType)
		ProjectileParent(0.3f, loc, velocity, entityQueryHandler)
	{}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
private:
};