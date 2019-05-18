#pragma once

#include "ProjectileParent.h"

class ProjectileTwo : public ProjectileParent {
public:
	ProjectileTwo(const Vecf3& loc, const Vecf3& velocity, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		// (projectileSize, projectileLocation, velocity)
		ProjectileParent(0.2f, loc, velocity, entityQueryHandler)
	{}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
private:
};