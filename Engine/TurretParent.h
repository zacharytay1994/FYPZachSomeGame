#pragma once

#include "Entity.h"

#include <vector>

struct ProjectileData {
	const int projectileType;
	Vecf3 targetLocationInWorld;
};

class TurretParent : public Entity {
public:
	// explicitly set height of turret
	TurretParent(const float& size, const Veci3& loc, const float& worldSize, const int& gridSize, const int& rateOfFire)
		:
		Entity(size, loc, worldSize, gridSize),
		rateOfFire(rateOfFire)
	{}
	// implicitly set height of turret based on heightmap
	TurretParent(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, const int& rateOfFire)
		:
		Entity(size, loc, heightDisplaced, worldSize, gridSize),
		rateOfFire(rateOfFire)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw() = 0;
	enum class ProjectileType {
		ProjectileOne,
		ProjectileTwo,
		ProjectileThree
	};
	virtual void SpawnProjectile(const ProjectileType& type) {
		struct ProjectileData data = { static_cast<int>(type), targetLocation };
		ProjectileData.push_back(data);
	}
	virtual void FireAtRate(const ProjectileType& type) {
		if (clock < timePerProjectile) {
			clock += 1.0f / 60.0f;
		}
		else {
			SpawnProjectile(type);
			clock = 0.0f;
		}
	}
public:
	std::vector<int> ProjectileHolder;
	std::vector<Vecf3> VelocityHolder;
	std::vector<ProjectileData> ProjectileData;
	int rateOfFire;
	float clock = 0.0f;
	float timePerProjectile = 1.0f / rateOfFire;
	Vecf3 targetLocation = {0.0f, 0.0f, 0.0f};
	bool hasTarget;
};