#pragma once

#include "TurretParent.h"
#include "EnemyParent.h"
#include "BuildingParent.h"
#include "ProjectileParent.h"
#include "Quadtree.h"

#include <memory>
#include <map>

class EntityQueryHandler {
public:
	EntityQueryHandler(std::vector <std::unique_ptr<TurretParent>>& turretBuffer,
		std::vector <std::unique_ptr<EnemyParent>>& enemyBuffer,
		std::vector <std::unique_ptr<BuildingParent>>& buildingBuffer,
		std::vector <std::unique_ptr<ProjectileParent>>& projectileBuffer,
		std::map<int, Entity*>& entityMap,
		Quadtree<ProjectileParent>& projectileQt,
		Quadtree<EnemyParent>& enemyQt)
		:
		TurretBuffer(turretBuffer),
		EnemyBuffer(enemyBuffer),
		BuildingBuffer(buildingBuffer),
		ProjectileBuffer(projectileBuffer),
		entityMap(entityMap),
		projectileQt(projectileQt),
		enemyQt(enemyQt)
	{}
	// returns squared distance between 2 points in world coordinates
	float DistanceBetween(const Vecf3& p1, const Vecf3& p2) {
		return (sq(p2.x - p1.x) + sq(p2.y - p1.y) + sq(p2.z - p1.z));
	}
	// return details of the nearest building found relative to a specific position in world coordinates from the building buffer, i.e. buildings in world
	bool QueryNearestBuilding(const Vecf3& fromLocation, Vecf3& toLocation, float& distanceAway, int& targetID) {
		if (BuildingBuffer.size() > 0) {
			bool buildingExists = false;
			float temp;
			Vecf3 nearestBuildingLocation;
			float nearestDistanceHolder = std::numeric_limits<float>::infinity();
			for (std::unique_ptr<BuildingParent>& e : BuildingBuffer) {
				if (!e->IsDestroyed()) {
					buildingExists = true;
					temp = DistanceBetween(fromLocation, e->GetSpawnLocationOffset());
					if (temp < nearestDistanceHolder) {
						nearestDistanceHolder = temp;
						nearestBuildingLocation = e->GetSpawnLocationOffset();
						targetID = e->GetUniqueID();
					}
				}
			}
			if (buildingExists) {
				distanceAway = nearestDistanceHolder;
				toLocation = nearestBuildingLocation;
				return true;
			}
			return false;
		}
		return false;
	}
	// returns pointer to entity of targetID taken from entityMap
	Entity* GetEntityFromID(const int& entity) {
		return entityMap[entity];
	}
	// returns nearest entity reference relative to a position in world
	bool QueryNearestEnemy(const Vecf3 fromLocation, Entity*& entity) {
		if (EnemyBuffer.size() > 0) {
			bool enemyExists = false;
			float temp;
			float nearestDistanceHolder = std::numeric_limits<float>::infinity();
			for (std::unique_ptr<EnemyParent>& e : EnemyBuffer) {
				if (!e->IsDead()) {
					enemyExists = true;
					temp = DistanceBetween(fromLocation, e->GetSpawnLocationOffset());
					if (temp < nearestDistanceHolder) {
						nearestDistanceHolder = temp;
						entity = e.get();
					}
				}
			}
			if (enemyExists) {
				return true;
			}
			return false;
		}
		return false;
	}
	// returns number of projectiles that overlap with an entity relative to its size, i.e aabb 
	int CheckProjectileEnemyCollision(Entity* entity) {
		int hits = 0;
		projectileQt.QueryQt(Rect(Vecf2(entity->GetSpawnLocationOffset().x, entity->GetSpawnLocationOffset().z), entity->GetSize(), entity->GetSize()), hits, entity->GetSpawnLocationOffset().y, entity->GetSize()/2.0f);
		return hits;
	}
	// returns a container of enemy positions within range of this enemy
	std::vector<EnemyParent*> GetEnemiesWithinRange(EnemyParent* entity) {
		return enemyQt.GetElements(Rect(Vecf2(entity->GetSpawnLocationOffset().x, entity->GetSpawnLocationOffset().z), entity->GetSize() * 5.0f, entity->GetSize() * 5.0f), entity);
	}
private:
	std::vector <std::unique_ptr<TurretParent>>& TurretBuffer;
	std::vector <std::unique_ptr<EnemyParent>>& EnemyBuffer;
	std::vector <std::unique_ptr<BuildingParent>>& BuildingBuffer;
	std::vector <std::unique_ptr<ProjectileParent>>& ProjectileBuffer;

	// stores all entities in the world with <key:uniqueEntityID, value:Entity*>
	std::map<int, Entity*>& entityMap;
	// quadtree data structure used to store projectiles for positional queries
	Quadtree<ProjectileParent>& projectileQt;
	Quadtree<EnemyParent>& enemyQt;
};