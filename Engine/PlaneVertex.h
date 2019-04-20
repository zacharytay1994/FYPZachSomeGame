#pragma once

#include "Vec2.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"

class PlaneVertex {
public:
	template<class T>
	static IndexedTriangleList<T> GetPlane(float size) {
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

		std::vector<T> vertexVec(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertexVec[i].pos = vertices[i];
			vertexVec[i].texpos = textureCoords[i];
		}

		return { std::move(vertexVec), {0,2,1, 2,3,1} };
	}

	template<class T>
	static IndexedTriangleList<T> GetPlaneHorizontal(float size) {
		const float side = size / 2.0f;

		std::vector<Vecf3> vertices;
		std::vector<Vecf2> textureCoords;

		vertices.emplace_back(-side, 0.0f, -side);
		textureCoords.emplace_back(0.0f, 1.0f);
		vertices.emplace_back(side, 0.0f, -side);
		textureCoords.emplace_back(1.0f, 1.0f);
		vertices.emplace_back(-side, 0.0f, side);
		textureCoords.emplace_back(0.0f, 0.0f);
		vertices.emplace_back(side, 0.0f, side);
		textureCoords.emplace_back(1.0f, 0.0f);

		std::vector<T> vertexVec(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertexVec[i].pos = vertices[i];
			vertexVec[i].texpos = textureCoords[i];
		}

		return { std::move(vertexVec), {0,2,1, 2,3,1} };
	}
};