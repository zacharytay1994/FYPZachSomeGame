#pragma once

#include "Pathfinding.h"
#include "Terrain.h"
#include "Pipeline.h"
#include "IndexedTriangleList.h"
#include "SurfaceDirectionalLighting.h"
#include "MatTemplate.h"
#include "Vec3.h"

#include <string>
#include <cmath>
#include <sstream>

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
	void SyncWithWorldEntities(std::vector<std::unique_ptr<Entity>>& solidBuffer) {
		pathfinding.UpdateGridObstacles(solidBuffer);
	}
	// renders terrain
	void Draw(const Matf4& worldTransform, const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		// binding transformation matrices to rendering pipeline
		groundPipeline->effect.vertexShader.BindWorld(worldTransform);
		groundPipeline->effect.vertexShader.BindView(viewMatrix);
		groundPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		groundPipeline->Draw(terrain.terrainList);
	}
	// draws path found by pathfinding object
	void DrawPath(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		pathfinding.DrawGridPath(viewMatrix, projectionMatrix);
	}
	// Finds the path
	bool FindPath(const Vecf3& startPos, const Vecf3& endPos) {
		return pathfinding.FindPath(startPos, endPos);
	}

	std::vector<Vecf3> GetPathInWorldCoordinates() {
		return pathfinding.GetPointsOfPath();
	}

	bool FindAndReturnPath(const Vecf3& startPos, const Vecf3& endPos, std::vector<Vecf3>& pathInOut) {
		if (FindPath(startPos, endPos)) {
			pathInOut = GetPathInWorldCoordinates();
			return true;
		}
		return false;
	}

	std::shared_ptr<HeightMap>& GetHeightMap() {
		return terrain.GetHeightMap();
	}

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

	bool QueryQuadCollisionEstimate(const Vecf3& locationIn, ProjectileParent* projectile) {
		// get rounded cell location
		int gridX = std::clamp(int(std::trunc((worldSize / 2 + locationIn.x) / unitsPerCell)), 0, 99);
		int gridZ = std::clamp(gridSize - int(std::trunc((worldSize / 2 + locationIn.z) / unitsPerCell)), 0, 99);
		float terrainHeight = terrain.terrainList.vertices[gridZ * (gridSize + 1) + gridX].pos.y;
		if (locationIn.y < terrainHeight) {
			projectile->SetSpawnLocationOffsetY(terrainHeight);
			projectile->ApplyExternalForce(surfaceNormalList[gridZ * gridSize + gridX]);
			return true;
		}
		return false;
	}
public:
	const int gridSize;
private:
	Pathfinding pathfinding;
	Terrain terrain;
	// rendering stuffs
	std::unique_ptr<Pipeline<SurfaceDirectionalLighting>> groundPipeline;
	std::vector<Vecf3> surfaceNormalList;
	const float worldSize;
	const float unitsPerCell;
	std::wstringstream ss;
};
