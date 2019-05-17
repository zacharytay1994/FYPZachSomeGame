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
#include "Quadtree.h"
#include "ConsoleBox.h"

#include <vector>
#include <memory>
#include <random>

#include <sstream>
#include <string>

class EntityHandler {
	typedef typename SurfaceDirectionalLighting::PixelShader::EntityType TextureEntityType;
public:
	EntityHandler(Graphics& gfx, std::shared_ptr<ZBuffer>& zbuffer, const float& worldSize, const int& gridSize,
		std::shared_ptr<TerrainWithPath>& terrainWithPath, std::shared_ptr<ConsoleBox>& consoleBox) 
		:
		entityZBuffer(zbuffer),
		entityPipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, entityZBuffer)),
		worldSize(worldSize),
		gridSize(gridSize),
		terrainWithPath(terrainWithPath),
		projectileQt(worldSize),
		gfx(gfx),
		consoleBox(consoleBox)
	{
		// initialized the effect used by the pipeline, non static textures, red for enemy, green for turret, and blue for buildings
		entityPipeline->effect.pixelShader.SetStaticTexture(false);
		entityPipeline->effect.pixelShader.AddTexture("redimage.bmp");
		entityPipeline->effect.pixelShader.AddTexture("greenimage.bmp");
		entityPipeline->effect.pixelShader.AddTexture("blueimage.bmp");
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
			ReadDebugQueue((*x)->debugQueue);
			if (enemyBuffer.size() > 0) {
				(*x)->targetLocation = QueryEnemyLocation(*(enemyBuffer.begin()));
			}
		}
		// update projectile entities buffer
		projectileQt.Reset();
		std::vector<std::unique_ptr<ProjectileParent>>::iterator pEnd = projectileBuffer.end();
		for (std::vector<std::unique_ptr<ProjectileParent>>::iterator x = projectileBuffer.begin(); x != pEnd; std::advance(x, 1)) {
			/*if ((x != pEnd) && (*x)->toDestroy) {
				QuickRemoveProjectile(projectileBuffer, x, pEnd);
			}
			if (x != pEnd) {
				(*x)->Update(kbd, mouse, dt);
			}*/
			(*x)->Update(kbd, mouse, dt);
			ReadDebugQueue((*x)->debugQueue);
			// check if projectile collides with the terrain surface
			terrainWithPath->QueryQuadCollisionEstimate((*x)->GetSpawnLocationOffset(), (*x).get());

			// reset colliding flag of projectile
			(*x)->isColliding = false;
			// add projectile into the projectile quadtree
			projectileQt.InsertElement((*x).get());
		}
		// update enemy entities buffer
		std::vector<std::unique_ptr<EnemyParent>>::iterator eEnd = enemyBuffer.end();
		for (std::vector<std::unique_ptr<EnemyParent>>::iterator x = enemyBuffer.begin(); x != eEnd; std::advance(x, 1)) {
			(*x)->Update(kbd, mouse, dt);
			ReadDebugQueue((*x)->debugQueue);
			//ReadDebugQueue((*x)->debugQueue);
			// check if enemy needs new path, find it a new path
			/*if ((*x)->needPath) {
				QueryPathfinder((*x));
			}*/
		}
		// entity handler functions
		GetProjectilesFromTurrets();
		if (enemyBuffer.size() > 0) {
			// queries the quadtree with temporary aabb of enemy entities
			projectileQt.QueryQt(Rect(Vecf2(enemyBuffer[0]->GetSpawnLocationOffset().x, enemyBuffer[0]->GetSpawnLocationOffset().z), 4.0f, 4.0f));
			// set query range variable of quadtree object for debugging purposes
			projectileQt.SetQueryRange(Rect(Vecf2(enemyBuffer[0]->GetSpawnLocationOffset().x, enemyBuffer[0]->GetSpawnLocationOffset().z), 4.0f, 4.0f));
		}
		// Write entity debug messages to console
		WriteEDMToConsole();
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
			//(*x)->Draw(viewMatrix, projectionMatrix);
		}
		// loop through and render turret buffer
		entityPipeline->effect.pixelShader.SetTextureType(TextureEntityType::Turret);
		std::vector<std::unique_ptr<TurretParent>>::iterator tEnd = turretBuffer.end();
		for (std::vector<std::unique_ptr<TurretParent>>::iterator x = turretBuffer.begin(); x != tEnd; std::advance(x, 1)) {
			translateVector = (*x)->GetSpawnLocationOffset();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
		}
		// loop through and render projectile buffer
		std::vector<std::unique_ptr<ProjectileParent>>::iterator pEnd = projectileBuffer.end();
		for (std::vector<std::unique_ptr<ProjectileParent>>::iterator x = projectileBuffer.begin(); x != pEnd; std::advance(x, 1)) {
			translateVector = (*x)->GetSpawnLocationOffset();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			entityPipeline->effect.vertexShader.BindWorld(worldTransform);
			entityPipeline->Draw((*x)->GetCubeList());
		}
		// loop through and render enemy entities
		entityPipeline->effect.pixelShader.SetTextureType(TextureEntityType::Enemy);
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
		//OutputDebugString(ss.str().c_str());
	}
	void DrawDebugDisplay() {
		projectileQt.DrawNodeDebuggingDisplay(gfx.ScreenWidth/2, gfx);
	}
	void SetHeightMap(std::shared_ptr<HeightMap>& heightmapIn) {
		heightmap = heightmapIn;
	}
	// functions to add entities into the world (size, location) { -----
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
	void AddEnemy(const float& size, const Veci2& loc) {
		float temp = heightmap->heightDisplacementGrid[loc.y*heightmap->width + loc.x];
		enemyBuffer.emplace_back(std::make_unique<EnemyOne>(size, loc, temp, worldSize, gridSize));
		(*(enemyBuffer.end() - 1))->Calculate3DLocationOffset();
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
			height = heightmap->heightDisplacementGrid[gridLocation.y*heightmap->width + gridLocation.x] + 0.1f;
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
	// ----- }

	// gets projectiles stored in projectile buffers, waiting to be spawned, of turrets, and spawns it
	void GetProjectilesFromTurrets() {
		// loop through all turrets
		std::vector<std::unique_ptr<TurretParent>>::iterator end = turretBuffer.end();
		std::vector<ProjectileData>::iterator projectileDataEnd;
		for (std::vector<std::unique_ptr<TurretParent>>::iterator x = turretBuffer.begin(); x != end; std::advance(x, 1)) {
			projectileDataEnd = (*x)->ProjectileData.end();
			for (std::vector<ProjectileData>::iterator start = (*x)->ProjectileData.begin(); start != projectileDataEnd; std::advance(start, 1)) {
				switch ((*start).projectileType) {
					// spawn projectile one
				case 0:
					projectileBuffer.emplace_back(std::make_unique<ProjectileOne>((*x)->GetSpawnLocationOffset(), (*start).velocity));
					break;
					// spawn projectile two
				case 1:
					projectileBuffer.emplace_back(std::make_unique<ProjectileTwo>((*x)->GetSpawnLocationOffset(), (*start).velocity));
					break;
					// spawn projectile three
				case 2:
					projectileBuffer.emplace_back(std::make_unique<ProjectileThree>((*x)->GetSpawnLocationOffset(), (*start).velocity));
					break;
				}
			}
			// clears projectile buffer held by the turret, i.e. turret has no more projectiles waiting to be spawned
			(*x)->ProjectileData.clear();
		}
	}
	// finds a path for an enemy entity to get from its current location to its target destination
	// if path found, set its temporary needPath state to false, true if otherwise
	void QueryPathfinder(std::unique_ptr<EnemyParent>& enemy) {
		std::vector<Vecf3> holder;
		if (terrainWithPath->FindAndReturnPath(enemy->GetSpawnLocationOffset(), enemy->targetDestination, holder)) {
			enemy->SetCurrentPath(holder);
			enemy->needPath = false;
		}
	}
	// get the location in world coordinates of an enemy entity in the world
	Vecf3 QueryEnemyLocation(std::unique_ptr<EnemyParent>& enemy) {
		return enemy->GetSpawnLocationOffset();
	}
	// incomplete and unused at the moment,
	// quick removal of elements from a vector container without requiring it to reallocate memory
	void QuickRemoveProjectile(std::vector<std::unique_ptr<ProjectileParent>>& container, std::vector<std::unique_ptr<ProjectileParent>>::iterator& elementIterator,
		std::vector<std::unique_ptr<ProjectileParent>>::iterator& updateEnd) {
		std::vector<std::unique_ptr<ProjectileParent>>::iterator lastElement = container.end() - 1;
		if (elementIterator != container.end()) {
			elementIterator = std::move(lastElement);
		}
		container.pop_back();
		updateEnd = container.end();
	}
	void ReadDebugQueue(std::queue<Entity::DebugMessage>& debugQueue) {
		while (debugQueue.size() != 0) {
			debugMessagePQ.push(debugQueue.front());
			debugQueue.pop();
		}
	}
	void WriteEDMToConsole() {
		while (debugMessagePQ.size() != 0) {
			consoleBox->Write(debugMessagePQ.top().message + " /g@t:" + std::to_string((float)debugMessagePQ.top().timeElapsed / 1000.0f) + "s");
			debugMessagePQ.pop();
		}
	}
public:
	// buffer that holds all solid inanimate entities in the world
	std::vector <std::unique_ptr<Entity>> solidBuffer;
	// world variables
	const float worldSize;
	const int gridSize;
private:
	// outdated buffer used to hold generic any entities in the world
	std::vector<std::unique_ptr<Entity>> entityBuffer;
	// buffer that holds all turrets in the world
	std::vector<std::unique_ptr<TurretParent>> turretBuffer;
	// buffer that holds all projectiles in the world
	std::vector<std::unique_ptr<ProjectileParent>> projectileBuffer;
	// enemy buffers
	// buffer that holds all enemies in the world that are executing a path
	std::vector<std::unique_ptr<EnemyParent>> enemyBuffer;
	// buffer that holds all enemies in the world that are waiting for a path
	std::vector<std::unique_ptr<EnemyParent>> enemiesAwaitingPath;
	
	// reference to height displacement map of the world
	std::shared_ptr<HeightMap> heightmap;
	std::shared_ptr<TerrainWithPath>& terrainWithPath;
	// console box
	std::shared_ptr<ConsoleBox>& consoleBox;
	// reverse priority queue with smallest time elapsed at the top (typealloc, container, comparatoroperator)
	std::priority_queue<Entity::DebugMessage, std::vector<Entity::DebugMessage>, Entity::DebugMessage> debugMessagePQ;
	 
	// graphics reference used for debugging overlay of quadtree
	Graphics& gfx;
	// quadtree data structure used to store projectiles for positional queries
	Quadtree<ProjectileParent> projectileQt;

	// rendering pipeline stuff 
	std::shared_ptr<ZBuffer> entityZBuffer;
	std::shared_ptr<Pipeline<SurfaceDirectionalLighting>> entityPipeline;
	Vecf3 translateVector;
	Matf4 worldTransform;

	// string stream for debugging purposes
	std::wstringstream ss;
};