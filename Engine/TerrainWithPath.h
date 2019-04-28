#pragma once

#include "Pathfinding.h"
#include "Terrain.h"
#include "Pipeline.h"
#include "IndexedTriangleList.h"
#include "SurfaceDirectionalLighting.h"
#include "MatTemplate.h"
#include "Vec3.h"

#include <string>

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

	void SyncTerrainWithHeightmap() {
		pathfinding.BindHeightMap(terrain);
	}

	void SyncWithWorldEntities(std::vector<std::unique_ptr<Entity>>& solidBuffer) {
		pathfinding.UpdateGridObstacles(solidBuffer);
	}

	void Draw(const Matf4& worldTransform, const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		groundPipeline->effect.vertexShader.BindWorld(worldTransform);
		groundPipeline->effect.vertexShader.BindView(viewMatrix);
		groundPipeline->effect.vertexShader.BindProjection(projectionMatrix);

		groundPipeline->Draw(terrain.terrainList);
		pathfinding.DrawGridPath(viewMatrix, projectionMatrix);
	}

	void FindPath(const Vecf3& startPos, const Vecf3& endPos) {
		pathfinding.FindPath(startPos, endPos);
	}

	void DrawFoundPath(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		pathfinding.DrawGridPath(viewMatrix, projectionMatrix);
	}
private:
	Pathfinding pathfinding;
	Terrain terrain;
	// rendering stuffs
	std::unique_ptr<Pipeline<SurfaceDirectionalLighting>> groundPipeline;
	const float worldSize;
	const int gridSize;
	//const float cellRadius = (float)((worldSize / gridSize) / 2.0f);
};
