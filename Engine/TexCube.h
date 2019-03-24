#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"

class TexCube {
public:
	template<class T>
	static IndexedTriangleList<T> GetWrap(float size) {
		const float side = size / 2.0f;

		std::vector<Vecf3> vertices;
		std::vector<Vecf2> textureCoords;

		vertices.emplace_back(-side, -side, -side);
		textureCoords.emplace_back(0.0f, 1.0f);
		vertices.emplace_back(side, -side, -side);
		textureCoords.emplace_back(1.0f, 1.0f);
		vertices.emplace_back(-side, side, -side);
		textureCoords.emplace_back(0.0f, 0.0f);
		vertices.emplace_back(side, side, -side);
		textureCoords.emplace_back(1.0f, 0.0f);
		vertices.emplace_back(-side, -side, side);
		textureCoords.emplace_back(1.0f, 1.0f);
		vertices.emplace_back(side, -side, side);
		textureCoords.emplace_back(0.0f, 1.0f);
		vertices.emplace_back(-side, side, side);
		textureCoords.emplace_back(1.0f, 0.0f);
		vertices.emplace_back(side, side, side);
		textureCoords.emplace_back(0.0f, 0.0f);

		std::vector<T> vertexVec(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertexVec[i].pos = vertices[i];
			vertexVec[i].texpos = textureCoords[i];
		}

		return { std::move(vertexVec), {0,2,1, 2,3,1, 1,3,5, 3,7,5, 2,6,3, 3,6,7, 4,5,7, 4,7,6, 0,4,2, 2,4,6, 0,1,4, 1,5,4} };
	}

	template<class T>
	static IndexedTriangleList<T> GetPlain(float size = 1.0f) {
		const float side = size / 2.0f;

		std::vector<Vecf3> vertices;
		std::vector<Vecf2> textureCoords;

		vertices.emplace_back(-side, -side, -side);
		textureCoords.emplace_back(0.0f, 1.0f);
		vertices.emplace_back(side, -side, -side);
		textureCoords.emplace_back(1.0f, 1.0f);
		vertices.emplace_back(-side, side, -side);
		textureCoords.emplace_back(0.0f, 0.0f);
		vertices.emplace_back(side, side, -side);
		textureCoords.emplace_back(1.0f, 0.0f);
		vertices.emplace_back(-side, -side, side);
		textureCoords.emplace_back(1.0f, 1.0f);
		vertices.emplace_back(side, -side, side);
		textureCoords.emplace_back(0.0f, 1.0f);
		vertices.emplace_back(-side, side, side);
		textureCoords.emplace_back(1.0f, 0.0f);
		vertices.emplace_back(side, side, side);
		textureCoords.emplace_back(0.0f, 0.0f);

		std::vector<T> vertexVec(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertexVec[i].pos = vertices[i];
		}

		return { std::move(vertexVec), {0,2,1, 2,3,1, 1,3,5, 3,7,5, 2,6,3, 3,6,7, 4,5,7, 4,7,6, 0,4,2, 2,4,6, 0,1,4, 1,5,4} };
	}
};