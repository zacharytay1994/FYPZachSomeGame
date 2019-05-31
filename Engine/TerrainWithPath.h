#pragma once

#include "Pathfinding.h"
#include "Terrain.h"
#include "Pipeline.h"
#include "IndexedTriangleList.h"
#include "SurfaceDirectionalLighting.h"
#include "MatTemplate.h"
#include "Vec3.h"

#include "ProjectileParent.h"
#include "EnemyParent.h"

#include <string>
#include <cmath>
#include <sstream>
#include <assert.h>

// mainly just a wrapper to wrap both Terrain.h and Pathfinding.h neatly together
class TerrainWithPath {
public:
	TerrainWithPath(Graphics& gfx, std::shared_ptr<ZBuffer>& zBuffer, const std::string& heightmap, const std::string& groundtexture, const float& worldSize, const int& gridSize, const float& minHeight, const float& maxHeight)
		:
		pathfinding(gfx, zBuffer, worldSize, gridSize, (worldSize / (float)gridSize) / 2.0f),
		terrain(heightmap, worldSize, gridSize, minHeight, maxHeight),
		groundPipeline(std::make_unique<Pipeline<SurfaceDirectionalLighting>>(gfx, zBuffer)),
		worldSize(worldSize),
		gridSize(gridSize),
		unitsPerCell(worldSize/gridSize)
	{
		groundPipeline->effect.pixelShader.BindTexture(groundtexture);
		SyncTerrainWithHeightmap();
		surfaceNormalList = CalculateSurfaceNormalMap();
	}
	// updates pathfinding grid with height buffer generated from a height map from terrain
	void SyncTerrainWithHeightmap() {
		pathfinding.BindHeightMap(terrain);
	}
	// updates pathfinding grid with solid obstacle entities to recognize untraversable terrain
	void SyncWithWorldEntities(std::vector<std::unique_ptr<BuildingParent>>& solidBuffer) {
		pathfinding.UpdateGridObstacles(solidBuffer);
	}
	// renders terrain
	void Draw(const Matf4& worldTransform, const Matf4& viewMatrix, const Matf4& projectionMatrix, const Matf4& reflectionViewMatrix) {
		// binding transformation matrices to rendering pipeline
		groundPipeline->effect.vertexShader.BindWorld(worldTransform);
		groundPipeline->effect.vertexShader.BindView(viewMatrix);
		groundPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		groundPipeline->effect.vertexShader.BindReflectionView(reflectionViewMatrix);
		groundPipeline->Draw(terrain.terrainList);
	}
	// draws path found by pathfinding object
	void DrawPath(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		pathfinding.DrawGridPath(viewMatrix, projectionMatrix);
	}
	// Finds the path
	bool FindPath(const Vecf3& startPos, const Vecf3& endPos, const float& radiusBuffer) {
		return pathfinding.FindPath(startPos, endPos, radiusBuffer);
	}
	// converts path in node position to world coordinates
	std::vector<Vecf3> GetPathInWorldCoordinates() {
		return pathfinding.GetPointsOfPath();
	}
	// queries a path and if found, return the path as a container of world coordinates (waypoints)
	bool FindAndReturnPath(const Vecf3& startPos, const Vecf3& endPos, std::vector<Vecf3>& pathInOut, const float& radiusBuffer) {
		if (FindPath(startPos, endPos, radiusBuffer)) {
			pathInOut.push_back(startPos);
			std::vector<Vecf3> temp = GetPathInWorldCoordinates();
			for (Vecf3 v : temp) {
				pathInOut.push_back(v);
			}
			pathInOut.push_back(endPos);
			return true;
		}
		return false;
	}
	// get the terrain heightmap
	std::shared_ptr<HeightMap>& GetHeightMap() {
		return terrain.GetHeightMap();
	}
	// calculate and returns a container of the surface normal of triangular quads that make up the surface
	// returns a grid of size [y*gridSize*2 + x] because a grid cell composes of 2 triangles, hence doubling the horizontal size
	std::vector<Vecf3> CalculateSurfaceNormalMap() {
		std::vector<Vecf3> surfaceNormalHolder;
		std::vector<size_t>::iterator end = terrain.terrainList.indices.end();
		for (std::vector<size_t>::iterator start = terrain.terrainList.indices.begin(); start != end; std::advance(start, 6)) {
			// calculating and emplacing back surfacenormal of left most triangle in square
			surfaceNormalHolder.push_back(((terrain.terrainList.vertices[*(start + 1)].pos - terrain.terrainList.vertices[*(start)].pos) %
				(terrain.terrainList.vertices[*(start + 2)].pos - terrain.terrainList.vertices[*(start)].pos)).GetNormalized());
			// calculating and emplacing back surfacenormal of right most triangle in square
			surfaceNormalHolder.push_back(((terrain.terrainList.vertices[*(start + 4)].pos - terrain.terrainList.vertices[*(start + 3)].pos) %
				(terrain.terrainList.vertices[*(start + 5)].pos - terrain.terrainList.vertices[*(start + 3)].pos)).GetNormalized());
		}
		return surfaceNormalHolder;
	}
	// queries a coordinate in world space to see if it falls below the surface, if so applies a rebounding force
	// together with other relevant forces to simulate collision with the surface, if not returns false
	bool QueryQuadCollisionEstimate(const Vecf3& locationIn, ProjectileParent* projectile) {
		// get rounded cell location
		int gridX = std::clamp(int(std::trunc((worldSize / 2 + locationIn.x) / unitsPerCell)), 0, gridSize - 1);
		int gridZ = std::clamp(gridSize - int(std::trunc((worldSize / 2 + locationIn.z) / unitsPerCell)), 0, gridSize - 1);
		// gets the terrain height with rounded cell location, i.e. height of the closest estimated vertex
		float terrainHeight = terrain.terrainList.vertices[gridZ * (gridSize + 1) + gridX].pos.y;
		// calculating which triangle in a square it is
		float excessX = locationIn.x - gridX * unitsPerCell;
		float excessY = locationIn.z - gridZ * unitsPerCell;
		// value used to determine if triangle falls on the right of left side of a square
		float sideDeterminant = excessX + excessY;
		if (locationIn.y < terrainHeight) {
			projectile->SetSpawnLocationOffsetY(terrainHeight + 0.1f);
			// calculate external force
			Vecf3 surfaceNormal;
			// right triangle in square
			if (sideDeterminant > unitsPerCell) {
				// get surface normal from surface normal list
				surfaceNormal = surfaceNormalList[gridZ * (gridSize*2) + gridX * 2 + 1];
			}
			// left triangle in square
			else {
				surfaceNormal = surfaceNormalList[gridZ * (gridSize*2) + gridX * 2];
			}
			Vecf3 normalForce = surfaceNormal * (-projectile->GetVelocity() * surfaceNormal);
			// applying the normal force, i.e. the normalalized surface perpendicular vector * perpendicular magnitude of incoming object
			// multiplied by 2 to achieve an elastic rebound
			projectile->ApplyExternalForce(normalForce * 2);
			// applying dampening and frictional force
			Vecf3 frictionalForce = -((projectile->GetVelocity() + normalForce) * 0.1f);
			// if entity velocity is above a set threshold, apply force
			if (abs(projectile->GetVelocity().x) > 0.1f && abs(projectile->GetVelocity().z) > 0.1f && abs(projectile->GetVelocity().y) > 0.1f) {
				projectile->ApplyExternalForce(frictionalForce);
				projectile->ApplyExternalForce(-projectile->GetVelocity() * 0.1f);
			}
			// else set its velocity to rest
			else {
				projectile->stop = true;
			}
			return true;
		}
		return false;
	}
	// queries a coordinate in world space to see if it falls below the surface, if so applies a rebounding force
	// together with other relevant forces to simulate collision with the surface, if not returns false
	// for enemies temporarily for testing
	bool QueryQuadCollisionEstimateEnemy(const Vecf3& locationIn, EnemyParent* projectile) {
		// get rounded cell location
		int gridX = std::clamp(int(std::trunc((worldSize / 2 + locationIn.x) / unitsPerCell)), 0, gridSize - 1);
		int gridZ = std::clamp(gridSize - int(std::trunc((worldSize / 2 + locationIn.z) / unitsPerCell)), 0, gridSize - 1);
		// gets the terrain height with rounded cell location, i.e. height of the closest estimated vertex
		float terrainHeight = terrain.terrainList.vertices[gridZ * (gridSize + 1) + gridX].pos.y;
		// calculating which triangle in a square it is
		float excessX = locationIn.x - gridX * unitsPerCell;
		float excessY = locationIn.z - gridZ * unitsPerCell;
		// value used to determine if triangle falls on the right of left side of a square
		float sideDeterminant = excessX + excessY;
		float yOffset = projectile->GetSize() / 2.0f;
		if ((locationIn.y - yOffset) < terrainHeight) {
			projectile->SetSpawnLocationOffsetYControlled(terrainHeight + yOffset);
			// calculate external force
			Vecf3 surfaceNormal;
			// right triangle in square
			if (sideDeterminant > unitsPerCell) {
				// get surface normal from surface normal list
				surfaceNormal = surfaceNormalList[gridZ * (gridSize * 2) + gridX * 2 + 1];
			}
			// left triangle in square
			else {
				surfaceNormal = surfaceNormalList[gridZ * (gridSize * 2) + gridX * 2];
			}
			Vecf3 normalForce = surfaceNormal * (-projectile->currentVelocity * surfaceNormal);
			// applying the normal force, i.e. the normalalized surface perpendicular vector * perpendicular magnitude of incoming object
			// multiplied by 2 to achieve an elastic rebound
			projectile->ApplyExternalForce(normalForce);
			// applying dampening and frictional force
			//Vecf3 frictionalForce = -((projectile->currentVelocity + normalForce) * 0.1f);
			//// if entity velocity is above a set threshold, apply force
			//if (abs(projectile->currentVelocity.x) > 0.1f && abs(projectile->currentVelocity.z) > 0.1f && abs(projectile->currentVelocity.y) > 0.1f) {
			//	projectile->ApplyExternalForce(frictionalForce);
			//	projectile->ApplyExternalForce(-projectile->currentVelocity * 0.1f);
			//}
			//// else set its velocity to rest
			//else {
			//	projectile->stop = true;
			//}
			return true;
		}
		return false;
	}
	NodeAStar* GetGridCell(const Veci2& cell) {
		assert(cell.x < gridSize && cell.x > 0 && cell.y < gridSize && cell.y > 0);
		return pathfinding.grid->grid[cell.y*gridSize + cell.x].get();
	}
	float GetCellDiameter() {
		return worldSize / gridSize;
	}
public:
	// size of the grid, i.e. number of square tiles (2 triangular quads) making up the world
	const int gridSize;
private:
	// object that executes the A* seach algorithm
	Pathfinding pathfinding;
	// object that holds the relevant details of the world terrain
	Terrain terrain;
	// list holding the calculate surface normals of individual quads that make up the terrain
	std::vector<Vecf3> surfaceNormalList;
	// the world size in terms of model space units
	const float worldSize;
	// the ratio of gridSize : worldSize
	const float unitsPerCell;

	// rendering stuffs
	std::unique_ptr<Pipeline<SurfaceDirectionalLighting>> groundPipeline;

	// string stream for debugging purposes
	std::wstringstream ss;
};
