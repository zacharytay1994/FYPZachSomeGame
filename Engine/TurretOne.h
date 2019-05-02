#pragma once

#include "TurretParent.h"
#include "ProjectileOne.h"
#include "Physics.h"

#include <sstream>
#include <string>

class TurretOne : public TurretParent {
public:
	TurretOne(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize)
		:
		TurretParent(size, loc, heightDisplaced, worldSize, gridSize)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) override {
		if (kbd.KeyIsPressed('P')) {
			SpawnProjectile(ProjectileType::ProjectileOne, spawnLocationOffset + Vecf3(2.0f, 0.0f, 2.0f));
		}
		if (kbd.KeyIsPressed('O')) {
			SpawnProjectile(ProjectileType::ProjectileOne, spawnLocationOffset + Vecf3(-4.0f, 0.0f, 0.0f));
		}
		if (kbd.KeyIsPressed('I')) {
			SpawnProjectile(ProjectileType::ProjectileOne, spawnLocationOffset + Vecf3(-2.0f, 0.0f, 0.0f));
		}
		SpawnProjectile(ProjectileType::ProjectileOne, spawnLocationOffset + Vecf3(1.0f, 4.0f, -5.0f));
	}
	virtual void Draw() override {

	}
private:
	std::wstringstream ss;
};