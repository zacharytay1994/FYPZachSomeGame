#pragma once

#include "TurretParent.h"
#include "ProjectileOne.h"

class TurretOne : public TurretParent {
public:
	TurretOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize)
		:
		TurretParent(size, loc, heightDisplaced, worldSize, gridSize)
	{}
	virtual void Update() override {
		LoadProjectile(ProjectileType::ProjectileOne);
	}
	virtual void Draw() override {

	}
	void LoadProjectile(ProjectileType p) {
		ProjectileHolder.push_back(static_cast<int>(p));
	}
private:
};