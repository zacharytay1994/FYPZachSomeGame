#pragma once

#include "Pipeline.h"
#include "IndexedTriangleList.h"

#include <memory>
#include <vector>

class Tile {
public:
	Tile(int type) 
		:
		type(type)
	{}
private:
	int type;
};

template <class T>
class Board {
public:
	Board(const int& dimensions) 
		:
		dimensions(dimensions),
		tileList(TexCube::GetWrapIndependent<Pipeline<SurfaceDirectionalLighting>::Vertex>(1.0f))
	{
		for (int i = 0; i < size; i++) {
			tiles.emplace_back(std::make_unique<Tile>(0));
		}
	}
	~Board() = default;
	void DrawBoard(std::shared_ptr<Pipeline<T>>& pipeline, const Matf4& orientation) {
		int i = 0;
		int j = 0;
		Vecf3 offset;
		for (curTile = tiles.begin(), i = 0, j = 0; curTile != tiles.end(); std::advance(curTile, 1), i++, i % width == 0 ? j++ : j+=0) {
			offset = startDimensions + Vecf3(float(i % width), 0.0f, float(j));
			worldTransform = Matf4::Translation(offset) * orientation;
			pipeline->effect.vertexShader.BindWorld(worldTransform);
			pipeline->Draw(tileList);
		}
	}
	void SetDimensions(const int& d) {
		dimensions = d;
	}
public:
	std::vector<std::unique_ptr<Tile>>::iterator curTile;
private:
	int dimensions;
	int width = dimensions * 2;
	int size = width * width;
	Vecf3 boardOrigin = { 0.5f, 0.0f, 2.0f };
	Vecf3 startDimensions = boardOrigin - Vecf3(float(dimensions), 0.0f, float(dimensions));
	Matf4 worldTransform = Matf4::Identity();
	std::vector<std::unique_ptr<Tile>> tiles;
	IndexedTriangleList<Pipeline<SurfaceDirectionalLighting>::Vertex> tileList;
};