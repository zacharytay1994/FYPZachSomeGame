#pragma once

#include "TextureVertex.h"
#include "IndexedTriangleList.h"

class TexCube {
public:
	TexCube(const float size) {
		const float side = size / 2.0f;
		texVertices.emplace_back(TextureVertex({ -side, -side, -side }, { 0,1 }));
		texVertices.emplace_back(TextureVertex({ side, -side, -side }, { 1, 1 }));
		texVertices.emplace_back(TextureVertex({ -side, side, -side }, { 0, 0 }));
		texVertices.emplace_back(TextureVertex({ side, side, -side }, { 1, 0 }));
		texVertices.emplace_back(TextureVertex({ -side, -side, side }, { 1, 1 }));
		texVertices.emplace_back(TextureVertex({ side, -side, side }, { 0, 1 }));
		texVertices.emplace_back(TextureVertex({ -side, side, side }, { 1, 0 }));
		texVertices.emplace_back(TextureVertex({ side, side, side }, { 0, 0 }));
	}
	IndexedTriangleList getTriangleList() const {
		return { texVertices, {0,2,1, 2,3,1, 1,3,5, 3,7,5, 2,6,3, 3,6,7, 4,5,7, 4,7,6, 0,4,2, 2,4,6, 0,1,4, 1,5,4} };
	}

	std::vector<TextureVertex> texVertices;
};