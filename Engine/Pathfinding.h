#pragma once

#include "GridAStar.h"
#include "Graphics.h"
#include "Entity.h"
#include "MatTemplate.h"
#include "Terrain.h"
#include "ZBuffer.h"

#include <vector>
#include <algorithm>
#include <queue>
#include <utility>

class Pathfinding {
public:
	Pathfinding(Graphics& gfx, std::shared_ptr<ZBuffer>& zBuffer)
		:
		grid(std::make_unique<GridAStar>(gfx, 10.0f, zBuffer))
	{}

	void FindPath(const Vecf3& startPos, const Vecf3& endPos) {
		// custom comparator for priority queue, max heap to min heap
		auto nodePointerGreaterComparator = [](NodeAStar*& p1, NodeAStar* p2) {
			return *p1 > *p2;
		};

		// create min heap
		std::priority_queue<NodeAStar*, std::vector<NodeAStar*>, decltype(nodePointerGreaterComparator)> minHeap(nodePointerGreaterComparator);

		// calls .get() on unique_ptrs in grid, to create temp stored_ptrs
		NodeAStar* startNode = grid->NodeFromWorldPosition(startPos);
		NodeAStar* endNode = grid->NodeFromWorldPosition(endPos);

		// add start node to the open set
		minHeap.push(startNode);
		startNode->SetInOpen(true);
		nodesToReset.push_back(startNode);

		// loop
		while (minHeap.size() > 0) {
			NodeAStar* currentNode = minHeap.top();
			
		    //erase currentNode from openSet
			minHeap.pop();
			currentNode->SetInOpen(false);
			// add currentNode to closedSet
			currentNode->SetInClosed(true);

			// if currentNode is equal to endNode, we have reached our destination
			if (currentNode == endNode) {
				currentPath = RetracePath(startNode, endNode);
				return;
			}

			// find/update fcost of neighbours and add them to the openset
			for (NodeAStar* n : grid->GetNeighbours(currentNode)) {
				// if not traversable or already in closeSet or too high to traverse, skip
				if (!n->GetWalkable() || n->GetInClosed() || TooHighToTraverse(currentNode, n) || TooLowToTraverse(currentNode, n)) {
					continue;
				}
				// if newgCost < gCost (need updating), or if not in openset, update/calculate fcost, add to openset
				// calculate new gCost of neighbour based on currentNode
				int newGCost = currentNode->gCost + GetDistanceBetweenNodes(currentNode, n);
				// check
				bool inOpenSet = n->GetInOpen();
				if (newGCost < n->gCost || !inOpenSet) {
					// set fcost, by setting gCost and hCost
					n->gCost = newGCost;
					n->hCost = GetDistanceBetweenNodes(n, endNode);
					// set parent node
					n->SetParent(currentNode);
					// if not in openset, add to openset
					if (!inOpenSet) {
						minHeap.push(n);
						n->SetInOpen(true);
					}
					nodesToReset.push_back(n);
				}
			}
		}
	}

	// finding distance between nodes
	int GetDistanceBetweenNodes(NodeAStar*& node1, NodeAStar*& node2) {
		int distanceX = abs(node1->gridX - node2->gridX);
		int distanceY = abs(node1->gridY - node2->gridY);

		if (distanceX > distanceY) {
			return distanceY * 14 + (distanceX - distanceY) * 10;
		}
		return distanceX * 14 + (distanceY - distanceX) * 10;
	}

	// retrace path using parent nodes
	std::vector<NodeAStar*> RetracePath(NodeAStar* startNode, NodeAStar* endNode) {
		std::vector<NodeAStar*> path;
		NodeAStar* currentNode = endNode;

		// traversing backwards through parent
		while (startNode != currentNode) {
			currentNode->SetVisualize(true);
			path.push_back(currentNode);
			currentNode = currentNode->GetParent();
		}

		std::reverse(path.begin(), path.end());
		for (NodeAStar* n : nodesToReset) {
			ResetNode(n);
		}
		nodesToReset.clear();
		return path;
	}

	void ResetNode(NodeAStar*& node) {
		node->gCost = 0;
		node->hCost = 0;
		node->SetInClosed(false);
		node->SetInOpen(false);
		//node->SetVisualize(false);
	}

	// update grid with obstacle entities from solidBuffer in entityHandler
	void UpdateGridObstacles(std::vector<std::unique_ptr<Entity>>& solidBuffer) {
		grid->UpdateWalkable(solidBuffer);
	}

	// draw all grid cells
	void DrawGrid(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		grid->Draw(viewMatrix, projectionMatrix);
	}
	// draw on path grid cells
	void DrawGridPath(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		grid->DrawPath(viewMatrix, projectionMatrix, currentPath);
	}

	void RedefineGrid(const float& radius, std::vector<std::unique_ptr<Entity>>& solidBuffer) {
		grid->RedefineGrid(radius, solidBuffer);
	}

	void BindHeightMap(Terrain& terrain) {
		std::vector<float>& heightDisplacementGrid = terrain.GetHeightDisplacementGrid();
		for (int y = 0; y < 100; y++) {
			for (int x = 0; x < 100; x++) {
				grid->grid[y * 100 + x]->heightValue = heightDisplacementGrid[y * 101 + x];
				grid->grid[y * 100 + x]->UpdateWorldPos(heightDisplacementGrid[y * 101 + x]);
			}
		}
	}

	bool TooHighToTraverse(NodeAStar*& currentNode, NodeAStar*& neighbour) {
		if ((neighbour->heightValue - currentNode->heightValue) > maxVerticalTraverse) {
			return true;
		}
		return false;
	}

	bool TooLowToTraverse(NodeAStar*& currentNode, NodeAStar*& neighbour) {
		if ((neighbour->heightValue - currentNode->heightValue) < -maxVerticalTraverse) {
			return true;
		}
		return false;
	}

private:
	std::unique_ptr<GridAStar> grid;
	std::vector<NodeAStar*> currentPath;
	// nodes to reset
	std::vector<NodeAStar*> nodesToReset;
	// vertical heuristics
	float maxVerticalTraverse = 0.2;
};