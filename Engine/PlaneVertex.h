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

	template<class T>
	static IndexedTriangleList<T> GetPlaneHorizontalSplit(const float& size, const int& split) {
		const float side = size / 2.0f;

		std::vector<Vecf3> vertices;
		std::vector<Vecf2> textureCoords;

		// find and emplace back vertices
		float startX = (-size / 2.0f);
		float startY = (size / 2.0f);
		float diameter = size / split;
		int noOfVertices = (split + 1)*(split + 1);
		int x = 0;
		int y = 0;
		for (x = 0, y = 0; (y + 1)*split + (x + 1) <= noOfVertices; x = (x + 1) % (split + 1), x == 0 ? y++ : y += 0) {
			vertices.emplace_back((float)(startX + x * diameter), 0.0f, (float)(startY - y * diameter));
			textureCoords.emplace_back((float)x/split, (float)y/split);
		}

		// filling out indices list
		std::vector<size_t> indicesList;
		x = 0;
		y = 0;
		for (x = 0, y = 0; (y + 1)*split + (x + 1) < noOfVertices; x = (x + 1) % (split + 1), x == 0 ? y++ : y += 0) {
			if (x < split && y < split) {
				indicesList.push_back((y)*(split + 1) + x);
				indicesList.push_back((y)*(split + 1) + (x+1));
				indicesList.push_back((y + 1)*(split + 1) + x);

				indicesList.push_back((y)*(split + 1) + (x+1));
				indicesList.push_back((y + 1)*(split + 1) + (x + 1));
				indicesList.push_back((y + 1)*(split + 1) + x);
			}
		}

		std::vector<T> vertexVec(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertexVec[i].pos = vertices[i];
			vertexVec[i].texpos = textureCoords[i];
		}

		return { std::move(vertexVec), std::move(indicesList) };
	}
};