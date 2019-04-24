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
	bool operator>(NodeAStar& rhs) {
		if (rhs.GetfCost() < GetfCost()) {
			return true;
		}
		return false;
	}
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
	bool GetInClosed() {
		return inClosed;
	}
	void SetInClosed(bool boolIn) {
		inClosed = boolIn;
	}
	bool GetInOpen() {
		return inOpen;
	}
	void SetInOpen(bool boolIn) {
		inOpen = boolIn;
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
	// open and closed set
	bool inClosed = false;
	bool inOpen = false;
};