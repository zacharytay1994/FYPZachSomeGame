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
		for (float x = gridStartPos.x + nodeRadius; x < gridEndX; x += nodeDiameter) {
			for (float y = gridStartPos.y - nodeRadius; y > gridEndZ; y -= nodeDiameter) {
				grid.emplace_back(std::make_unique<NodeAStar>(true, Vecf3( x, nodeRadius, y )));
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
	void Draw(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		gridPipeline->effect.vertexShader.BindView(viewMatrix);
		gridPipeline->effect.vertexShader.BindProjection(projectionMatrix);
		std::vector<std::unique_ptr<NodeAStar>>::iterator gridEnd = grid.end();
		for (std::vector<std::unique_ptr<NodeAStar>>::iterator j = grid.begin(); j != gridEnd; std::advance(j, 1)) {
			translateVector = (*j)->GetWorldPos();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			gridPipeline->effect.vertexShader.BindWorld(worldTransform);
			if ((*j)->GetWalkable()) {
				gridPipeline->SetIsOccupied(false);
			}
			else if (!(*j)->GetWalkable()) {
				gridPipeline->SetIsOccupied(true);
			}
			gridPipeline->Draw(cubeList);
		}
	}
private:
	std::vector<std::unique_ptr<NodeAStar>> grid;
	const Vecf2 gridStartPos;
	const float gridSize;
	float nodeRadius = 0.5f;
	float nodeDiameter = nodeRadius*2;
	std::shared_ptr<WireframePipeline<SurfaceDirectionalLighting>> gridPipeline;
	IndexedTriangleList<WireframePipeline<SurfaceDirectionalLighting>::Vertex> cubeList;
	Vecf3 translateVector;
	Matf4 worldTransform;
};