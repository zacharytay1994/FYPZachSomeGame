#pragma once

#include "Vec3.h"

class NodeAStar {
public:
	NodeAStar(const bool& walkable, const Vecf3& worldPos) 
		:
		walkable(walkable),
		worldPos(worldPos)
	{}
	Vecf3 GetWorldPos() {
		return worldPos;
	}
	bool GetWalkable() {
		return walkable;
	}
	void SetWalkable(const bool& walkableIn) {
		walkable = walkableIn;
	}
private:
	bool walkable;
	Vecf3 worldPos;
};