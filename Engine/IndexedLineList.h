#pragma once

#include "Vec3.h"
#include <vector>

struct IndexedLineList {
	std::vector<Vecf3> vertices; // vertex buffer
	std::vector<size_t> indices; // index buffer
};