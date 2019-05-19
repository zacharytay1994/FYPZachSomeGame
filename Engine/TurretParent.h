#pragma once

#include "Entity.h"
#include "Physics.h"
#include "ProjectileCatalogue.h"

#include <vector>

struct ProjectileData {
	const int projectileType;
	Vecf3 velocity;
};

class TurretParent : public Entity {
public:
	// explicitly set height of turret
	TurretParent(const float& size, const Veci3& loc, const float& worldSize, const int& gridSize, const int& rateOfFire, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		Entity(size, loc, worldSize, gridSize, entityQueryHandler),
		rateOfFire(rateOfFire)
	{}
	// implicitly set height of turret based on heightmap
	TurretParent(const float& size, const Veci2& loc, const float& heightDisplaced, const float& worldSize, const int& gridSize, const int& rateOfFire, std::shared_ptr<EntityQueryHandler>& entityQueryHandler)
		:
		Entity(size, loc, heightDisplaced, worldSize, gridSize, entityQueryHandler),
		rateOfFire(rateOfFire)
	{}
	virtual void Update(Keyboard&kbd, Mouse& mouse, float dt) = 0;
	virtual void ChildMessage(const MessageDispatcher::Telegram& msg) override {}
	enum class ProjectileType {
		ProjectileOne,
		ProjectileTwo,
		ProjectileThree
	};
	virtual void SpawnProjectile(const ProjectileType& type) {
		Vecf3 velocityHolder;
		if (CalculateVelocity(velocityHolder, type)) {
			struct ProjectileData data = { static_cast<int>(type), velocityHolder };
			ProjectileData.push_back(data);
		}
		
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
	virtual bool CalculateAngleOfProjection(float& theta, const ProjectileType& type, const float& projectileSpeed) {
		if (Physics::GetProjectionAngleElevated(spawnLocationOffset, targetLocation, projectileSpeed, theta, ProjectileCatalogue::GetType(static_cast<int>(type)))) {
			return true;
		}
		return false;
	}
	virtual bool CalculateVelocity(Vecf3& velocityHolder, const ProjectileType& type) {
		float theta;
		float projectileSpeed = ProjectileCatalogue::GetSpeed(static_cast<int>(type));
		if (CalculateAngleOfProjection(theta, type, projectileSpeed)) {
			float speedX;
			float speedZ;
			float diagonalDistance;
			float xDisplacement = targetLocation.x - spawnLocationOffset.x;
			float diagonalSpeed = projectileSpeed * cos(theta);
			if (xDisplacement != 0) {
				diagonalDistance = sqrt(sq(targetLocation.x - spawnLocationOffset.x) + sq(targetLocation.z - spawnLocationOffset.z));
				speedX = ((targetLocation.x - spawnLocationOffset.x) * (diagonalSpeed)) / diagonalDistance;
				speedZ = ((targetLocation.z - spawnLocationOffset.z) * (diagonalSpeed)) / diagonalDistance;
			}
			else {
				speedX = 0.0f;
				speedZ = diagonalSpeed;
			}

			velocityHolder = { speedX, projectileSpeed * sin(theta), speedZ };
			return true;
		}
		return false;
	}
public:
	std::vector<ProjectileData> ProjectileData;
	int rateOfFire;
	float clock = 0.0f;
	float timePerProjectile = 1.0f / rateOfFire;
	Vecf3 targetLocation = {0.0f, 0.0f, 0.0f};
	bool hasTarget;
	float maxRange = 12.0f;

	// enemy target lock
	bool targetFound = false;
	Entity* target;
};