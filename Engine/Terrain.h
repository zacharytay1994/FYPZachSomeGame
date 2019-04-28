#pragma once

#include "HeightMap.h"
#include "IndexedLineList.h"
#include "Pipeline.h"
#include "SurfaceDirectionalLighting.h"
#include "PlaneVertex.h"

#include <string>

// handles terrain generation, in other words, world positions of ground vertices
class Terrain {
public:
	Terrain(const std::string& string, const float& worldSize, const int& gridSize, const float& minHeight, const float& maxHeight) 
		:
		terrainList(PlaneVertex::GetPlaneHorizontalSplit<Pipeline<SurfaceDirectionalLighting>::Vertex>(worldSize, gridSize)),
		heightmap(string, gridSize + 1, gridSize + 1, minHeight, maxHeight)
	{
		AlterVertices();
	}
	// alter vertices y position based on heightmap
	void AlterVertices() {
		for (int x = 0; x < heightmap.width; x++) {
			for (int y = 0; y < heightmap.height; y++) {
				terrainList.vertices[y*heightmap.width + x].pos.y = (float)(heightmap.heightDisplacementGrid[y*heightmap.width + x]);
			}
		}
	}
	// returns a reference to height displacement grid, mainly to alter nodes/gridcells in GridAStar
	std::vector<float>& GetHeightDisplacementGrid() {
		return heightmap.heightDisplacementGrid;
	}
public:
	// list of vertices and indices used by the rendering pipeline
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> terrainList;
private:
	HeightMap heightmap;
};