#pragma once

#include "Entity.h"

class BuildingParent : public Entity {
public:
	BuildingParent(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		Entity(size, loc, heightDisplaced + size / 4.0f, worldSize, gridSize, entityQueryHandler)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt)
	{
		ChildUpdates(kbd, mouse, dt);
	}
	virtual void ChildUpdates(Keyboard&kbd, Mouse& mouse, float dt) = 0;
private:
};