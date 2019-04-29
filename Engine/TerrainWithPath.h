#pragma once

#include "Pathfinding.h"
#include "Terrain.h"
#include "Pipeline.h"
#include "IndexedTriangleList.h"
#include "SurfaceDirectionalLighting.h"
#include "MatTemplate.h"
#include "Vec3.h"

#include <string>

// mainly just a wrapper to wrap both Terrain.h and Pathfinding.h neatly together
class TerrainWithPath {
public:
	TerrainWithPath(Graphics& gfx, std::shared_ptr<ZBuffer>& zBuffer, const std::string& heightmap, const std::string& groundtexture, const float& worldSize, const int& gridSize, const float& minHeight, const float& maxHeight)
		:
		pathfinding(gfx, zBuffer, worldSize, gridSize, (float)((worldSize / gridSize) / 2.0f)),
		terrain(heightmap, worldSize, gridSize, minHeight, maxHeight),
		groundPipeline(std::make_unique<Pipeline<SurfaceDirectionalLighting>>(gfx, zBuffer)),
		worldSize(worldSize),
		gridSize(gridSize)
	{
		groundPipeline->effect.pixelShader.BindTexture(groundtexture);
		SyncTerrainWithHeightmap();
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
	void FindPath(const Vecf3& startPos, const Vecf3& endPos) {
		pathfinding.FindPath(startPos, endPos);
	}

	std::shared_ptr<HeightMap>& GetHeightMap() {
		return terrain.GetHeightMap();
	}
public:
	const int gridSize;
private:
	Pathfinding pathfinding;
	Terrain terrain;
	// rendering stuffs
	std::unique_ptr<Pipeline<SurfaceDirectionalLighting>> groundPipeline;
	const float worldSize;
};
