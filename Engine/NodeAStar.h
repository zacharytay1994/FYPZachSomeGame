#pragma once

#include "Vec3.h"

class NodeAStar {
public:
	NodeAStar(const bool& walkable, const Vecf3& worldPos, const int& gridX, const int& gridY) 
		:
		walkable(walkable),
		worldPos(worldPos),
		gridX(gridX),
		gridY(gridY)
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
	int GetfCost() {
		return gCost + hCost;
	}
	void SetParent(NodeAStar*& parentIn) {
		parent = parentIn;
	}
	NodeAStar* GetParent() {
		return parent;
	}
	void SetVisualize(bool boolIn) {
		visualize = boolIn;
	}
	bool GetVisualize() {
		return visualize;
	}
public:
	// distance from start node to this node
	int gCost = 0;
	// distance from this node to end node
	int hCost = 0;
	const int gridX;
	const int gridY;
private:
	bool walkable;
	Vecf3 worldPos;
	// parent node
	NodeAStar* parent;
	bool visualize = false;
};