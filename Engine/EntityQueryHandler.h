#pragma once

#include "TurretParent.h"
#include "EnemyParent.h"
#include "BuildingParent.h"
#include "ProjectileParent.h"

#include <memory>
#include <map>

class EntityQueryHandler {
public:
	EntityQueryHandler(std::vector <std::unique_ptr<TurretParent>>& turretBuffer,
		std::vector <std::unique_ptr<EnemyParent>>& enemyBuffer,
		std::vector <std::unique_ptr<BuildingParent>>& buildingBuffer,
		std::vector <std::unique_ptr<ProjectileParent>>& projectileBuffer,
		std::map<int, Entity*>& entityMap)
		:
		TurretBuffer(turretBuffer),
		EnemyBuffer(enemyBuffer),
		BuildingBuffer(buildingBuffer),
		ProjectileBuffer(projectileBuffer),
		entityMap(entityMap)
	{}
	float DistanceBetween(const Vecf3& p1, const Vecf3& p2) {
		return (sq(p2.x - p1.x) + sq(p2.z - p1.z));
	}
	bool QueryNearestBuilding(const Vecf3& fromLocation, Vecf3& toLocation) {
		if (BuildingBuffer.size() > 0) {
			float temp;
			Vecf3 nearestBuildingLocation;
			float nearestDistanceHolder = std::numeric_limits<float>::infinity();
			for (std::unique_ptr<BuildingParent>& e : BuildingBuffer) {
				temp = DistanceBetween(fromLocation, e->GetSpawnLocationOffset());
				if (temp < nearestDistanceHolder) {
					nearestDistanceHolder = temp;
					nearestBuildingLocation = e->GetSpawnLocationOffset();
				}
			}
			toLocation = nearestBuildingLocation;
			return true;
		}
		return false;
	}
	Entity* GetEntityFromID(const int& entity) {
		return entityMap[entity];
	}
private:
	std::vector <std::unique_ptr<TurretParent>>& TurretBuffer;
	std::vector <std::unique_ptr<EnemyParent>>& EnemyBuffer;
	std::vector <std::unique_ptr<BuildingParent>>& BuildingBuffer;
	std::vector <std::unique_ptr<ProjectileParent>>& ProjectileBuffer;

	std::map<int, Entity*>& entityMap;
};