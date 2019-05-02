#pragma once

#include "Entity.h"
#include "EntityOne.h"
#include "TurretParent.h"
#include "TurretOne.h"
#include "ProjectileOne.h"

#include "HeightMap.h"
#include "Graphics.h"
#include "MatTemplate.h"

#include <vector>
#include <memory>

#include <sstream>
#include <string>

class EntityHandler {
public:
	EntityHandler(Graphics& gfx, std::shared_ptr<ZBuffer>& zbuffer, const float& worldSize, const int& gridSize) 
		:
		entityZBuffer(zbuffer),
		entityPipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, entityZBuffer)),
		worldSize(worldSize),
		gridSize(gridSize)
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
		}
		// update projectile entities buffer
		std::vector<std::unique_ptr<ProjectileParent>>::iterator pEnd = projectileBuffer.end();
		for (std::vector<std::unique_ptr<ProjectileParent>>::iterator x = projectileBuffer.begin(); x != pEnd; std::advance(x, 1)) {
			(*x)->Update(kbd, mouse, dt);
		}
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
		int counttest = 0;
		std::vector<std::unique_ptr<ProjectileParent>>::iterator pEnd = projectileBuffer.end();
		for (std::vector<std::unique_ptr<ProjectileParent>>::iterator x = projectileBuffer.begin(); x != pEnd; std::advance(x, 1)) {
			translateVector = (*x)->GetSpawnLocationOffset();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
			counttest++;
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
		ss << counttest << std::endl;
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
		turretBuffer.emplace_back(std::make_unique<TurretOne>(size, loc, temp, worldSize, gridSize));
		(*(turretBuffer.end() - 1))->Calculate3DLocationOffset();
	}
	/*void AddProjectile(const float& size, const Vecf3& loc) {
		projectileBuffer.emplace_back(std::make_unique<ProjectileOne>(size, loc));
	}*/
	void GetProjectilesFromTurrets() {
		// loop through all turrets
		std::vector<std::unique_ptr<TurretParent>>::iterator end = turretBuffer.end();
		std::vector<ProjectileData>::iterator projectileDataEnd;
		for (std::vector<std::unique_ptr<TurretParent>>::iterator x = turretBuffer.begin(); x != end; std::advance(x, 1)) {
			projectileDataEnd = (*x)->ProjectileData.end();
			for (std::vector<ProjectileData>::iterator start = (*x)->ProjectileData.begin(); start != projectileDataEnd; std::advance(start, 1)) {
				switch ((*start).projectileType) {
				case 0:
					projectileBuffer.emplace_back(std::make_unique<ProjectileOne>((*x)->GetSpawnLocationOffset(), (*start).targetLocationInWorld));
					break;
				}
			}
			(*x)->ProjectileData.clear();
		}
	}
public:
	std::vector <std::unique_ptr<Entity>> solidBuffer;
private:
	std::vector<std::unique_ptr<Entity>> entityBuffer;
	std::vector<std::unique_ptr<TurretParent>> turretBuffer;
	std::vector<std::unique_ptr<ProjectileParent>> projectileBuffer;
	
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

	std::wstringstream ss;
};