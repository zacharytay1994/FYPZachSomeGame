#pragma once

#include "HeightMap.h"
#include "IndexedLineList.h"
#include "Pipeline.h"
#include "SurfaceDirectionalLighting.h"
#include "PlaneVertex.h"

#include <string>

class Terrain {
public:
	Terrain(const std::string& string, const float& worldSize, const int& gridSize, const float& minHeight, const float& maxHeight) 
		:
		terrainList(PlaneVertex::GetPlaneHorizontalSplit<Pipeline<SurfaceDirectionalLighting>::Vertex>(worldSize, gridSize)),
		heightmap(string, gridSize + 1, gridSize + 1, minHeight, maxHeight)
	{
		AlterVertices();
	}
	void AlterVertices() {
		for (int x = 0; x < heightmap.width; x++) {
			for (int y = 0; y < heightmap.height; y++) {
				terrainList.vertices[y*heightmap.width + x].pos.y = (float)(heightmap.heightDisplacementGrid[y*heightmap.width + x]);
			}
		}
	}
	std::vector<float>& GetHeightDisplacementGrid() {
		return heightmap.heightDisplacementGrid;
	}
public:
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> terrainList;
private:
	HeightMap heightmap;
};