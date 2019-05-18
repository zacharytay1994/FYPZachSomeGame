#pragma once

#include "TurretParent.h"
#include "Physics.h"

class TurretTwo : public TurretParent {
public:
	TurretTwo(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, const int& rateOfFire, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		TurretParent(size, loc, heightDisplaced, worldSize, gridSize, rateOfFire, entityQueryHandler)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		FireAtRate(ProjectileType::ProjectileTwo);
	}
private:
};