#pragma once

#include "Entity.h"

#include <vector>

struct ProjectileData {
	const int projectileType;
	Vecf3 targetLocationInWorld;
};

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
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual void Draw() = 0;
	enum class ProjectileType {
		ProjectileOne
	};
	virtual void SpawnProjectile(const ProjectileType& type, const Vecf3& targetLoc) {
		struct ProjectileData data = { static_cast<int>(type), targetLoc };
		ProjectileData.push_back(data);
	}
public:
	std::vector<int> ProjectileHolder;
	std::vector<Vecf3> VelocityHolder;
	std::vector<ProjectileData> ProjectileData;
};