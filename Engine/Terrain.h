#pragma once

#include "HeightMap.h"
#include "IndexedLineList.h"
#include "Pipeline.h"
#include "SurfaceDirectionalLighting.h"
#include "PlaneVertex.h"

#include <string>

class Terrain {
public:
	Terrain(const std::string& string) 
		:
		terrainList(PlaneVertex::GetPlaneHorizontalSplit<Pipeline<SurfaceDirectionalLighting>::Vertex>(10.0f, 100)),
		heightmap(string, 100 + 1, 100 + 1, 0.0f, 3.0f)
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
public:
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> terrainList;
private:
	HeightMap heightmap;
};