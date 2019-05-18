#pragma once

#include "ProjectileParent.h"

class ProjectileOne : public ProjectileParent{
public:
	ProjectileOne(const Vecf3& loc, const Vecf3& velocity, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		// (projectileSize, projectileLocation, targetLocation, projectileSpeed, projectionType)
		ProjectileParent(0.1f, loc, velocity, entityQueryHandler) 
	{
		//InsertDebugString("projectileone /y" + std::to_string(entityUniqueID) + " has been created.");
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) override {}
private:
};