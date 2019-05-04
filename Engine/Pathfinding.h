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

// mainly a*search pathfinding algorithm, and other basic functions
class Pathfinding {
public:
	Pathfinding(Graphics& gfx, std::shared_ptr<ZBuffer>& zBuffer, const float& worldSize, const int& gridSize, const float& gridCellRadius)
		:
		grid(std::make_unique<GridAStar>(gfx, worldSize, zBuffer, gridCellRadius)),
		nodeDiameter(gridCellRadius * 2.0f),
		gridSize(gridSize)
	{}
	// executes a*search algorithm and CALLS: this::RetracePath(), if path is found, nothing if not
	bool FindPath(const Vecf3& startPos, const Vecf3& endPos) {
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
				return true;
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
		return false;
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
	// resets nodes CALLS: this::ResetNode()
	std::vector<NodeAStar*> RetracePath(NodeAStar* startNode, NodeAStar* endNode) {
		// container to hold path trace
		std::vector<NodeAStar*> path;
		// starting from the end node
		NodeAStar* currentNode = endNode;

		// traversing backwards through parent nodes of each node back to the start node
		while (startNode != currentNode) {
			// for rendering purposes non relating to the algorithm
			currentNode->SetVisualize(true);
			// places current node in trace container
			path.push_back(currentNode);
			// current node traverses backwards to parent node
			currentNode = currentNode->GetParent();
		}
		// resets all changed attributes of nodes used in previous FindPath() calculation
		// CALLS: this::ResetNode()
		std::reverse(path.begin(), path.end());
		for (NodeAStar* n : nodesToReset) {
			ResetNode(n);
		}
		// clears nodesToReset container for next FindPath() call
		nodesToReset.clear();
		return path;
	}
	// resets all changed attributes of nodes
	// CALLEDBY: this::RetracePath()
	void ResetNode(NodeAStar*& node) {
		node->gCost = 0;
		node->hCost = 0;
		node->SetInClosed(false);
		node->SetInOpen(false);
		//node->SetVisualize(false);
	}

	// update grid with obstacle entities from solidBuffer in entityHandler, 
	// CALLS: GridAStar.h, CALLEDBY: TerrainWithPath::SyncWithWorldEntities
	void UpdateGridObstacles(std::vector<std::unique_ptr<Entity>>& solidBuffer) {
		grid->UpdateWalkable(solidBuffer);
	}

	// draw all grid cells
	// CALLS: GridAStar::Draw()
	void DrawGrid(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		grid->Draw(viewMatrix, projectionMatrix);
	}
	// draw on path grid cells
	// CALLS: GridAStar::DrawPath(), CALLEDBY: TerrainWithPath::Draw()
	void DrawGridPath(const Matf4& viewMatrix, const Matf4& projectionMatrix) {
		grid->DrawPath(viewMatrix, projectionMatrix, currentPath);
	}
	// redefine node grid, no longer used at the moment
	void RedefineGrid(const float& radius, std::vector<std::unique_ptr<Entity>>& solidBuffer) {
		grid->RedefineGrid(radius, solidBuffer);
	}
	// sets the height value of nodes in the world grid to their respective heights based on the heightmap used for terrain generation,
	// CALLEDBY: TerrainWithPath::SyncTerrainWithHeightMap()
	void BindHeightMap(Terrain& terrain) {
		std::vector<float>& heightDisplacementGrid = terrain.GetHeightDisplacementGrid();
		for (int y = 0; y < gridSize; y++) {
			for (int x = 0; x < gridSize; x++) {
				grid->grid[y * gridSize + x]->heightValue = heightDisplacementGrid[y * (gridSize + 1) + x];
				grid->grid[y * gridSize + x]->UpdateWorldPos(heightDisplacementGrid[y * (gridSize + 1) + x]);
			}
		}
	}
	// if distance between nodes upward is too large vertically to traverse
	// CALLEDBY this::FindPath()
	bool TooHighToTraverse(NodeAStar*& currentNode, NodeAStar*& neighbour) {
		if ((neighbour->heightValue - currentNode->heightValue) > maxUpwardTraversal) {
			return true;
		}
		return false;
	}
	// if distance between nodes downward is too large vertically to traverse
	// CALLEDBY this::FindPath()
	bool TooLowToTraverse(NodeAStar*& currentNode, NodeAStar*& neighbour) {
		if ((neighbour->heightValue - currentNode->heightValue) < -maxDownwardTraversal) {
			return true;
		}
		return false;
	}
	std::vector<Vecf3> GetPointsOfPath() {
		std::vector<Vecf3> holder;
		for (NodeAStar* n : currentPath) {
			holder.push_back(n->GetWorldPos());
		}
		return holder;
	}

private:
	// grid of nodes/gridcells used in pathfinding
	std::unique_ptr<GridAStar> grid;
	// stores the last path calculated by FindPath();
	std::vector<NodeAStar*> currentPath;
	// container of nodes used in previous calculations to reset
	std::vector<NodeAStar*> nodesToReset;
	// vertical heuristics
	float nodeDiameter;
	float maxUpwardTraversal = nodeDiameter * 2.0f;
	float maxDownwardTraversal = nodeDiameter * 2.0f;
	// grid size
	int gridSize;
};