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
	static IndexedTriangleList<T> GetWrapIndependent(float size) {
		const float side = size / 2.0f;

		std::vector<Vecf3> vertices;
		std::vector<Vecf2> textureCoords;

		const auto ConvertTexCoord = [](float u, float v)
		{
			return Vec2{ (u + 1.0f) / 3.0f,v / 4.0f };
		};

		vertices.emplace_back(-side, -side, -side); // 0
		textureCoords.emplace_back(ConvertTexCoord(1.0f, 0.0f));
		vertices.emplace_back(side, -side, -side); // 1
		textureCoords.emplace_back(ConvertTexCoord(0.0f, 0.0f));
		vertices.emplace_back(-side, side, -side); // 2
		textureCoords.emplace_back(ConvertTexCoord(1.0f, 1.0f));
		vertices.emplace_back(side, side, -side); // 3
		textureCoords.emplace_back(ConvertTexCoord(0.0f, 1.0f));
		vertices.emplace_back(-side, -side, side); // 4
		textureCoords.emplace_back(ConvertTexCoord(1.0f, 3.0f));
		vertices.emplace_back(side, -side, side); // 5
		textureCoords.emplace_back(ConvertTexCoord(0.0f, 3.0f));
		vertices.emplace_back(-side, side, side); // 6
		textureCoords.emplace_back(ConvertTexCoord(1.0f, 2.0f));
		vertices.emplace_back(side, side, side); // 7
		textureCoords.emplace_back(ConvertTexCoord(0.0f, 2.0f));
		vertices.emplace_back(-side, -side, -side); // 8
		textureCoords.emplace_back(ConvertTexCoord(1.0f, 4.0f));
		vertices.emplace_back(side, -side, -side); // 9
		textureCoords.emplace_back(ConvertTexCoord(0.0f, 4.0f));
		vertices.emplace_back(-side, -side, -side); // 10
		textureCoords.emplace_back(ConvertTexCoord(2.0f, 1.0f));
		vertices.emplace_back(-side, -side, side); // 11
		textureCoords.emplace_back(ConvertTexCoord(2.0f, 2.0f));
		vertices.emplace_back(side, -side, -side); // 12
		textureCoords.emplace_back(ConvertTexCoord(-1.0f, 1.0f));
		vertices.emplace_back(side, -side, side); // 13
		textureCoords.emplace_back(ConvertTexCoord(-1.0f, 2.0f));

		std::vector<T> vertexVec(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++) {
			vertexVec[i].pos = vertices[i];
			vertexVec[i].texpos = textureCoords[i];
		}

		return { std::move(vertexVec), {0,2,1, 2,3,1, 4,8,5, 5,8,9, 2,6,3, 3,6,7, 4,5,7, 4,7,6, 2,10,11, 2,11,6, 12,3,7, 12,7,13} };
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