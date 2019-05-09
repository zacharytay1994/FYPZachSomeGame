#pragma once

#include "Entity.h"
#include "EntityOne.h"

// types of turrets
#include "TurretParent.h"
#include "TurretOne.h"
#include "TurretTwo.h"
#include "TurretThree.h"

// types of projectiles
#include "ProjectileOne.h"
#include "ProjectileTwo.h"
#include "ProjectileThree.h"

// typed of enemies
#include "EnemyParent.h"
#include "EnemyOne.h"

#include "HeightMap.h"
#include "Graphics.h"
#include "MatTemplate.h"
#include "TerrainWithPath.h"

#include <vector>
#include <memory>
#include <random>

#include <sstream>
#include <string>

class EntityHandler {
public:
	EntityHandler(Graphics& gfx, std::shared_ptr<ZBuffer>& zbuffer, const float& worldSize, const int& gridSize,
		std::shared_ptr<TerrainWithPath>& terrainWithPath) 
		:
		entityZBuffer(zbuffer),
		entityPipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, entityZBuffer)),
		worldSize(worldSize),
		gridSize(gridSize),
		terrainWithPath(terrainWithPath)
	{
		entityPipeline->effect.pixelShader.BindTexture("greenimage.bmp");
	}

	void Update(Keyboard&kbd, Mouse& mouse, float dt) {
		// update normal entity buffer
		std::vector<std::unique_ptr<Entity>>::iterator end = entityBuffer.end();
		for (std::vector<std::unique_ptr<Entity>>::iterator x = entityBuffer.begin(); x != end; std::advance(x, 1)) {
			(*x)->Update(kbd, mouse, dt);
		}
		// update turret entities buffer
		std::vector<std::unique_ptr<TurretParent>>::iterator tEnd = turretBuffer.end();
		for (std::vector<std::unique_ptr<TurretParent>>::iterator x = turretBuffer.begin(); x != tEnd; std::advance(x, 1)) {
			(*x)->Update(kbd, mouse, dt);
			(*x)->targetLocation = QueryEnemyLocation(*(enemyBuffer.begin()));
		}
		// update projectile entities buffer
		std::vector<std::unique_ptr<ProjectileParent>>::iterator pEnd = projectileBuffer.end();
		for (std::vector<std::unique_ptr<ProjectileParent>>::iterator x = projectileBuffer.begin(); x != pEnd; std::advance(x, 1)) {
			/*if ((x != pEnd) && (*x)->toDestroy) {
				QuickRemoveProjectile(projectileBuffer, x, pEnd);
			}
			if (x != pEnd) {
				(*x)->Update(kbd, mouse, dt);
			}*/
			(*x)->Update(kbd, mouse, dt);
			float tempHeightHolder;
			if (terrainWithPath->QueryQuadCollision((*x)->GetSpawnLocationOffset(), tempHeightHolder) && !((*x)->stop)) {
				(*x)->SetSpawnLocationOffsetY(tempHeightHolder);
				(*x)->stop = true;
				/*ss.clear();
				ss << tempHeightHolder << std::endl;
				OutputDebugString(ss.str().c_str());*/
			}
		}
		// update enemy entities buffer
		std::vector<std::unique_ptr<EnemyParent>>::iterator eEnd = enemyBuffer.end();
		for (std::vector<std::unique_ptr<EnemyParent>>::iterator x = enemyBuffer.begin(); x != eEnd; std::advance(x, 1)) {
			(*x)->Update(kbd, mouse, dt);
			// check if enemy needs new path, find it a new path
			if ((*x)->needPath) {
				QueryPathfinder((*x));
			}
			/*float tempHeightHolder;
			terrainWithPath->QueryQuadCollision((*x)->GetSpawnLocationOffset(), tempHeightHolder);*/
			/*ss.clear();
			ss << (*x)->GetSpawnLocationOffset().y << std::endl;
			OutputDebugString(ss.str().c_str());*/
		}
		// entity handler functions
		GetProjectilesFromTurrets();
	}

	void Draw(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		entityPipeline->BeginFrame();
		entityPipeline->effect.vertexShader.BindView(viewMatrix);
		entityPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		
		// loop through and render normal entity buffer
		std::vector<std::unique_ptr<Entity>>::iterator end = entityBuffer.end();
		for (std::vector<std::unique_ptr<Entity>>::iterator x = entityBuffer.begin(); x != end; std::advance(x, 1)) {
			translateVector = (*x)->Calculate3DLocationOffset();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
		}
		// loop through and render turret buffer
		std::vector<std::unique_ptr<TurretParent>>::iterator tEnd = turretBuffer.end();
		for (std::vector<std::unique_ptr<TurretParent>>::iterator x = turretBuffer.begin(); x != tEnd; std::advance(x, 1)) {
			translateVector = (*x)->GetSpawnLocationOffset();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
		}
		// loop through and render projectile buffer
		//int counttest = 0;
		std::vector<std::unique_ptr<ProjectileParent>>::iterator pEnd = projectileBuffer.end();
		for (std::vector<std::unique_ptr<ProjectileParent>>::iterator x = projectileBuffer.begin(); x != pEnd; std::advance(x, 1)) {
			translateVector = (*x)->GetSpawnLocationOffset();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
			//counttest++;
		}
		// loop through and render enemy entities
		std::vector<std::unique_ptr<EnemyParent>>::iterator eEnd = enemyBuffer.end();
		for (std::vector<std::unique_ptr<EnemyParent>>::iterator x = enemyBuffer.begin(); x != eEnd; std::advance(x, 1)) {
			translateVector = (*x)->GetSpawnLocationOffset();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
		}
		// loop through and render solid entities
		end = solidBuffer.end();
		for (std::vector<std::unique_ptr<Entity>>::iterator x = solidBuffer.begin(); x != end; std::advance(x, 1)) {
			translateVector = (*x)->Calculate3DLocationOffset();
			(*x)->CalculateBoundaries();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
		}
		//ss << counttest << std::endl;
		//OutputDebugString(ss.str().c_str());
	}
	void SetHeightMap(std::shared_ptr<HeightMap>& heightmapIn) {
		heightmap = heightmapIn;
	}
	// add entity (size, location)
	void AddEntity(const float& size, const Veci2& loc) {
		entityBuffer.emplace_back(std::make_unique<EntityOne>(size, loc, worldSize, gridSize));
	}
	void AddEntity(const float& size, const Veci3& loc) {
		entityBuffer.emplace_back(std::make_unique<EntityOne>(size, loc, worldSize, gridSize));
	}
	void AddSolid(const float& size, const Veci2& loc) {
		solidBuffer.emplace_back(std::make_unique<EntityOne>(size, loc, worldSize, gridSize));
		(*(solidBuffer.end() - 1))->Calculate3DLocationOffset();
		(*(solidBuffer.end() - 1))->CalculateBoundaries();
	}
	void AddSolid(const float& size, const Veci3& loc) {
		solidBuffer.emplace_back(std::make_unique<EntityOne>(size, loc, worldSize, gridSize));
		(*(solidBuffer.end() - 1))->Calculate3DLocationOffset();
		(*(solidBuffer.end() - 1))->CalculateBoundaries();
	}
	void AddTurret(const float& size, const Veci2& loc) {
		float temp = heightmap->heightDisplacementGrid[loc.y*heightmap->width + loc.x];
		turretBuffer.emplace_back(std::make_unique<TurretOne>(size, loc, temp, worldSize, gridSize, 1));
		(*(turretBuffer.end() - 1))->Calculate3DLocationOffset();
	}
	void AddEnemy(const float& size, const Vecf3& loc) {
		enemyBuffer.emplace_back(std::make_unique<EnemyOne>(size, loc));
	}
	void PopulateRandomTurrets(const int& amount) {
		std::random_device rand;
		std::mt19937 rng(rand());
		std::uniform_int_distribution<std::mt19937::result_type> gridDistribution(0, 99);
		std::uniform_int_distribution<std::mt19937::result_type> rateOfFireDistribution(1, 5);
		std::uniform_int_distribution<std::mt19937::result_type> typeOfTurrets(0, 2);
		Veci2 gridLocation;
		float height;
		int rateOfFire;
		for (int i = 0; i < amount; i++) {
			gridLocation = { (int)(gridDistribution(rng)), (int)(gridDistribution(rng)) };
			rateOfFire = (int)(rateOfFireDistribution(rng));
			height = heightmap->heightDisplacementGrid[gridLocation.y*heightmap->width + gridLocation.x];
			switch (typeOfTurrets(rng)) {
				case 0:
					turretBuffer.emplace_back(std::make_unique<TurretOne>(0.5f, gridLocation, height, worldSize, gridSize, rateOfFire));
					(*(turretBuffer.end() - 1))->Calculate3DLocationOffset();
					break;
				case 1:
					turretBuffer.emplace_back(std::make_unique<TurretTwo>(0.5f, gridLocation, height, worldSize, gridSize, rateOfFire));
					(*(turretBuffer.end() - 1))->Calculate3DLocationOffset();
					break;
				case 2:
					turretBuffer.emplace_back(std::make_unique<TurretThree>(0.5f, gridLocation, height, worldSize, gridSize, rateOfFire));
					(*(turretBuffer.end() - 1))->Calculate3DLocationOffset();
					break;
			}
		}
	}
	void GetProjectilesFromTurrets() {
		// loop through all turrets
		std::vector<std::unique_ptr<TurretParent>>::iterator end = turretBuffer.end();
		std::vector<ProjectileData>::iterator projectileDataEnd;
		for (std::vector<std::unique_ptr<TurretParent>>::iterator x = turretBuffer.begin(); x != end; std::advance(x, 1)) {
			projectileDataEnd = (*x)->ProjectileData.end();
			for (std::vector<ProjectileData>::iterator start = (*x)->ProjectileData.begin(); start != projectileDataEnd; std::advance(start, 1)) {
				switch ((*start).projectileType) {
				case 0:
					projectileBuffer.emplace_back(std::make_unique<ProjectileOne>((*x)->GetSpawnLocationOffset(), (*start).velocity));
					break;
				case 1:
					projectileBuffer.emplace_back(std::make_unique<ProjectileTwo>((*x)->GetSpawnLocationOffset(), (*start).velocity));
					break;
				case 2:
					projectileBuffer.emplace_back(std::make_unique<ProjectileThree>((*x)->GetSpawnLocationOffset(), (*start).velocity));
					break;
				}
			}
			(*x)->ProjectileData.clear();
		}
	}
	void QueryPathfinder(std::unique_ptr<EnemyParent>& enemy) {
		std::vector<Vecf3> holder;
		if (terrainWithPath->FindAndReturnPath(enemy->GetSpawnLocationOffset(), enemy->targetDestination, holder)) {
			enemy->SetCurrentPath(holder);
			enemy->needPath = false;
		}
	}
	Vecf3 QueryEnemyLocation(std::unique_ptr<EnemyParent>& enemy) {
		return enemy->GetSpawnLocationOffset();
	}
	void QuickRemoveProjectile(std::vector<std::unique_ptr<ProjectileParent>>& container, std::vector<std::unique_ptr<ProjectileParent>>::iterator& elementIterator,
		std::vector<std::unique_ptr<ProjectileParent>>::iterator& updateEnd) {
		std::vector<std::unique_ptr<ProjectileParent>>::iterator lastElement = container.end() - 1;
		if (elementIterator != container.end()) {
			elementIterator = std::move(lastElement);
		}
		container.pop_back();
		updateEnd = container.end();
	}
public:
	std::vector <std::unique_ptr<Entity>> solidBuffer;
private:
	std::vector<std::unique_ptr<Entity>> entityBuffer;
	std::vector<std::unique_ptr<TurretParent>> turretBuffer;
	std::vector<std::unique_ptr<ProjectileParent>> projectileBuffer;
	// enemy buffers
	std::vector<std::unique_ptr<EnemyParent>> enemyBuffer;
	std::vector<std::unique_ptr<EnemyParent>> enemiesAwaitingPath;
	
	Vecf3 translateVector;
	Matf4 worldTransform;

	// pipeline stuff 
	std::shared_ptr<ZBuffer> entityZBuffer;
	std::shared_ptr<Pipeline<SurfaceDirectionalLighting>> entityPipeline;
	
	// reference to height displacement map of the world
	std::shared_ptr<HeightMap> heightmap;
	// world variables
	const float worldSize;
	const int gridSize;
	std::shared_ptr<TerrainWithPath>& terrainWithPath;

	std::wstringstream ss;
};