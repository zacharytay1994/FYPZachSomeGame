#pragma once

#include "NodeAStar.h"
#include "WireframePipeline.h"
#include "IndexedTriangleList.h"
#include "SurfaceDirectionalLighting.h"
#include "MatTemplate.h"
#include "Graphics.h"
#include "TexCube.h"
#include "Vec2.h"
#include "ZBuffer.h"
#include "Entity.h"

#include <vector>
#include <memory>
#include <algorithm>

// the grid used in the a*search algorithm and its relevant functions
class GridAStar {
public:
	GridAStar(Graphics& gfx, const float& worldSize, std::shared_ptr<ZBuffer>& zBuffer, const float& nodeRadius)
		:
		nodeRadius(nodeRadius),
		gridPipeline(std::make_shared<Pipeline<SurfaceDirectionalLighting>>(gfx, zBuffer)),
		cubeList(TexCube::GetPlain<Pipeline<SurfaceDirectionalLighting>::Vertex>(nodeDiameter)),
		worldSize(worldSize),
		gridStartPos(Vecf2(-worldSize /2, worldSize /2))
	{
		// finds grid cell end using grid cell start
		const float gridEndX = gridStartPos.x + worldSize;
		const float gridEndZ = gridStartPos.y - worldSize;
		int gridX = 0;
		int gridY = 0;
		// instantiates and emplaces nodes/gridcells based on specified node radius and world size
		for (float y = gridStartPos.y - nodeRadius, gridY = 0; y > gridEndZ; y -= nodeDiameter, gridY++) {
			for (float x = gridStartPos.x + nodeRadius, gridX = 0; x < gridEndX; x += nodeDiameter, gridX++) {
				grid.emplace_back(std::make_unique<NodeAStar>(true, Vecf3( x, nodeRadius, y ), (int)gridX, (int)gridY));
			}
		}
		NodeAStar* test = grid[3999].get();
		// bind texture to pipeline
		gridPipeline->effect.pixelShader.BindTexture("greenimage.bmp");
	}
	// updates nodes in grid array with walkable boolean based on solid entities in the world
	void UpdateWalkable(std::vector<std::unique_ptr<BuildingParent>>& solidBuffer) {
		std::vector<std::unique_ptr<BuildingParent>>::iterator end = solidBuffer.end();
		std::vector<std::unique_ptr<NodeAStar>>::iterator gridEnd = grid.end();
		for (std::vector<std::unique_ptr<NodeAStar>>::iterator j = grid.begin(); j != gridEnd; std::advance(j,1)) {
			(*j)->SetWalkable(true);
			for (std::vector<std::unique_ptr<BuildingParent>>::iterator i = solidBuffer.begin(); i != end; std::advance(i, 1)) {
				if ((*i)->PointCollide((*j)->GetWorldPos())) {
					(*j)->SetWalkable(false);
					(*j)->SetSolidCenter((*i)->GetSpawnLocationOffset());
				}
			}
		}
	}
	// redefines the grid parameters, not used anymore at the moment
	void RedefineGrid(const float& radius, std::vector<std::unique_ptr<BuildingParent>>& solidBuffer) {
		grid.clear();
		nodeRadius = radius;
		nodeDiameter = nodeRadius * 2;
		IndexedTriangleList<WireframePipeline<SurfaceDirectionalLighting>::Vertex> temp = TexCube::GetPlain<WireframePipeline<SurfaceDirectionalLighting>::Vertex>(nodeDiameter);
		cubeList.vertices = temp.vertices;
		cubeList.indices = temp.indices;
		gridCellSize = (int)(worldSize / nodeDiameter);
		const float gridEndX = gridStartPos.x + worldSize;
		const float gridEndZ = gridStartPos.y - worldSize;
		int gridX = 0;
		int gridY = 0;
		for (float y = gridStartPos.y - nodeRadius, gridY = 0; y > gridEndZ; y -= nodeDiameter, gridY++) {
			for (float x = gridStartPos.x + nodeRadius, gridX = 0; x < gridEndX; x += nodeDiameter, gridX++) {
				grid.emplace_back(std::make_unique<NodeAStar>(true, Vecf3(x, nodeRadius, y), (int)gridX, (int)gridY));
			}
		}
		UpdateWalkable(solidBuffer);
	}
	// draws entire grid with odes/gridcells being walkable changing the way the pipeline renders
	void Draw(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		gridPipeline->effect.vertexShader.BindView(viewMatrix);
		gridPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		std::vector<std::unique_ptr<NodeAStar>>::iterator gridEnd = grid.end();
		for (std::vector<std::unique_ptr<NodeAStar>>::iterator j = grid.begin(); j != gridEnd; std::advance(j, 1)) {
			translateVector = (*j)->GetWorldPos();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			gridPipeline->effect.vertexShader.BindWorld(worldTransform);
			gridPipeline->SetIsOccupied(!(*j)->GetWalkable());
			gridPipeline->SetVisualize((*j)->GetVisualize());
			gridPipeline->Draw(cubeList);
		}
	}
	// draws only nodes that have been passed in as a node on a defined path
	void DrawPath(const Matf4& viewMatrix, const Matf4& projectionMatrix, std::vector<NodeAStar*>& path) {
		gridPipeline->effect.vertexShader.BindView(viewMatrix);
		gridPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		std::vector<NodeAStar*>::iterator gridEnd = path.end();
		for (std::vector<NodeAStar*>::iterator j = path.begin(); j != gridEnd; std::advance(j, 1)) {
			translateVector = (*j)->GetWorldPos();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			gridPipeline->effect.vertexShader.BindWorld(worldTransform);
			gridPipeline->SetIsOccupied(!(*j)->GetWalkable());
			gridPipeline->SetVisualize((*j)->GetVisualize());
			gridPipeline->Draw(cubeList);
		}
	}
	// get node/gridcell position on grid based off of the world position
	NodeAStar* NodeFromWorldPosition(const Vecf3& worldPos) {
		float perX = std::clamp(((worldPos.x + worldSize /2) / worldSize), 0.0f, 1.0f);
		float perY = std::clamp((abs(worldPos.z - worldSize /2) / worldSize), 0.0f, 1.0f);

		int x = (int)(perX *(worldSize / nodeDiameter));
		int y = (int)(perY *(worldSize / nodeDiameter));
		
		int maxGridDimension = (int)(worldSize / nodeDiameter);
		return grid[y * maxGridDimension + x].get();
	}
	// get all neighbouring nodes of a node if they are within the grid dimensions
	std::vector<NodeAStar*> GetNeighbours(NodeAStar*& node) {
		std::vector<NodeAStar*> neighbours;
		// 3 by 3 grid search
		int nodeGridX;
		int nodeGridY;
		for (int x = - 1; x <= 1; x++) {
			for (int y = -1; y <= 1; y++) {
				// if center cell skip, not neighbour
				if (x == 0 && y == 0) {
					continue;
				}
				// if cell is within grid
				nodeGridX = node->gridX + x;
				nodeGridY = node->gridY + y;
				if (nodeGridX >= 0 && nodeGridX < gridCellSize &&
					nodeGridY >= 0 && nodeGridY < gridCellSize) {
					neighbours.push_back(grid[nodeGridY * gridCellSize + nodeGridX].get());
				}
			}
		}
		return neighbours;
	}
public:
	// container of nodes/gridcells
	std::vector<std::unique_ptr<NodeAStar>> grid;
private:
	// grid start position in terms of world position
	const Vecf2 gridStartPos;
	const float worldSize;
	float nodeRadius;
	float nodeDiameter = nodeRadius*2;
	// dimensions of grid, assuming square grid
	int gridCellSize = (int)(worldSize / nodeDiameter);
	// rendering objects
	std::shared_ptr<Pipeline<SurfaceDirectionalLighting>> gridPipeline;
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> cubeList;
	Vecf3 translateVector;
	Matf4 worldTransform;
};