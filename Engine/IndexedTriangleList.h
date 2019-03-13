#pragma once

#include <vector>
#include "Vec3.h"

struct IndexedTriangleList {
	std::vector<Vecf3> vertices;
	std::vector<size_t> indices;
};