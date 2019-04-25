#pragma once

#include "NodeAStar.h"
#include "WireframePipeline.h"
#include "IndexedTriangleList.h"
#include "SurfaceDirectionalLighting.h"
#include "MatTemplate.h"
#include "Graphics.h"
#include "TexCube.h"
#include "Vec2.h"

#include <vector>
#include <memory>
#include <algorithm>

class GridAStar {
public:
	GridAStar(Graphics& gfx, const float& planeSize)
		:
		gridPipeline(std::make_shared<WireframePipeline<SurfaceDirectionalLighting>>(gfx)),
		cubeList(TexCube::GetPlain<WireframePipeline<SurfaceDirectionalLighting>::Vertex>(nodeDiameter)),
		gridSize(planeSize),
		gridStartPos(Vecf2(-planeSize/2, planeSize/2))
	{
		const float gridEndX = gridStartPos.x + gridSize;
		const float gridEndZ = gridStartPos.y - gridSize;
		int gridX = 0;
		int gridY = 0;
		for (float x = gridStartPos.x + nodeRadius, gridX = 0; x < gridEndX; x += nodeDiameter, gridX++) {
			for (float y = gridStartPos.y - nodeRadius, gridY = 0; y > gridEndZ; y -= nodeDiameter, gridY++) {
				grid.emplace_back(std::make_unique<NodeAStar>(true, Vecf3( x, nodeRadius, y ), (int)gridX, (int)gridY));
			}
		}
	}
	void UpdateWalkable(std::vector<std::unique_ptr<Entity>>& solidBuffer) {
		std::vector<std::unique_ptr<Entity>>::iterator end = solidBuffer.end();
		std::vector<std::unique_ptr<NodeAStar>>::iterator gridEnd = grid.end();
		for (std::vector<std::unique_ptr<NodeAStar>>::iterator j = grid.begin(); j != gridEnd; std::advance(j,1)) {
			(*j)->SetWalkable(true);
			for (std::vector<std::unique_ptr<Entity>>::iterator i = solidBuffer.begin(); i != end; std::advance(i, 1)) {
				if ((*i)->PointCollide((*j)->GetWorldPos())) {
					(*j)->SetWalkable(false);
				}
			}
		}
	}
	void RedefineGrid(const float& radius, std::vector<std::unique_ptr<Entity>>& solidBuffer) {
		grid.clear();
		nodeRadius = radius;
		nodeDiameter = nodeRadius * 2;
		IndexedTriangleList<WireframePipeline<SurfaceDirectionalLighting>::Vertex> temp = TexCube::GetPlain<WireframePipeline<SurfaceDirectionalLighting>::Vertex>(nodeDiameter);
		cubeList.vertices = temp.vertices;
		cubeList.indices = temp.indices;
		gridCellSize = (int)(gridSize / nodeDiameter);
		const float gridEndX = gridStartPos.x + gridSize;
		const float gridEndZ = gridStartPos.y - gridSize;
		int gridX = 0;
		int gridY = 0;
		for (float x = gridStartPos.x + nodeRadius, gridX = 0; x < gridEndX; x += nodeDiameter, gridX++) {
			for (float y = gridStartPos.y - nodeRadius, gridY = 0; y > gridEndZ; y -= nodeDiameter, gridY++) {
				grid.emplace_back(std::make_unique<NodeAStar>(true, Vecf3(x, nodeRadius, y), (int)gridX, (int)gridY));
			}
		}
		UpdateWalkable(solidBuffer);
	}
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

	NodeAStar* NodeFromWorldPosition(const Vecf3& worldPos) {
		float perX = std::clamp(((worldPos.x + gridSize/2) / gridSize), 0.0f, 1.0f);
		float perY = std::clamp((abs(worldPos.z - gridSize/2) / gridSize), 0.0f, 1.0f);

		int x = (int)(perX *(gridSize / nodeDiameter));
		int y = (int)(perY *(gridSize / nodeDiameter));
		
		int maxGridDimension = (int)(gridSize / nodeDiameter);
		return grid[x * maxGridDimension + y].get();
	}

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
					neighbours.push_back(grid[nodeGridX * gridCellSize + nodeGridY].get());
				}
			}
		}
		return neighbours;
	}

private:
	std::vector<std::unique_ptr<NodeAStar>> grid;
	const Vecf2 gridStartPos;
	const float gridSize;
	float nodeRadius = 0.05f;
	float nodeDiameter = nodeRadius*2;
	int gridCellSize = (int)(gridSize / nodeDiameter);
	std::shared_ptr<WireframePipeline<SurfaceDirectionalLighting>> gridPipeline;
	IndexedTriangleList<WireframePipeline<SurfaceDirectionalLighting>::Vertex> cubeList;
	Vecf3 translateVector;
	Matf4 worldTransform;
};