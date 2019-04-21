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
				grid.emplace_back(NodeAStar(true, { x, nodeRadius, y }));
			}
		}
	}
	void Draw(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		gridPipeline->effect.vertexShader.BindView(viewMatrix);
		gridPipeline->effect.vertexShader.BindProjection(projectionMatrix);		
		for (NodeAStar a : grid) {
			translateVector = a.GetWorldPos();
			worldTransform = Matf4::RotationZ(0.0f) * Matf4::RotationX(0.0f) * Matf4::RotationY(0.0f) * Matf4::Translation(translateVector);
			gridPipeline->effect.vertexShader.BindWorld(worldTransform);
			gridPipeline->Draw(cubeList);
		}
	}
private:
	std::vector<NodeAStar> grid;
	const Vecf2 gridStartPos;
	const float gridSize;
	float nodeRadius = 1.0f;
	float nodeDiameter = nodeRadius*2;
	std::shared_ptr<WireframePipeline<SurfaceDirectionalLighting>> gridPipeline;
	IndexedTriangleList<WireframePipeline<SurfaceDirectionalLighting>::Vertex> cubeList;
	Vecf3 translateVector;
	Matf4 worldTransform;
};