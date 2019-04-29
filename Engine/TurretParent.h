#pragma once

#include "Entity.h"

class TurretParent : public Entity {
public:
	TurretParent(const float& size, const Veci3& loc, const float& worldSize, const int& gridSize)
		:
		Entity(size, loc, worldSize, gridSize)
	{}
	TurretParent(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize)
		:
		Entity(size, loc, heightDisplaced, worldSize, gridSize)
	{}
	virtual void Update() = 0;
	virtual void Draw() = 0;
private:
};