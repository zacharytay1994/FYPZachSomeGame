#pragma once

#include <vector>
#include "Vec3.h"
#include <assert.h>

template <class T>
struct IndexedTriangleList {
	IndexedTriangleList(std::vector<T> verts_in, std::vector<size_t> indices_in) 
		:
		vertices(std::move(verts_in)),
		indices(std::move(indices_in))
	{
		assert(vertices.size() > 2);
		assert(indices.size() % 3 == 0);
	}
	std::vector<T> vertices;
	std::vector<size_t> indices;
};