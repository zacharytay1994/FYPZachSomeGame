#pragma once

#include "GridAStar.h"

#include <sstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <utility>
#include <unordered_set>

#include <chrono>
#include <ctime>

class Pathfinding {
public:
	Pathfinding(const std::shared_ptr<GridAStar>& grid) 
		:
		grid(grid)
	{}

	void FindPath(const Vecf3& startPos, const Vecf3& endPos) {
		// custom comparator for priority queue
		auto nodePointerGreaterComparator = [](NodeAStar*& p1, NodeAStar* p2) {
			return *p1 > *p2;
		};

		std::priority_queue<NodeAStar*, std::vector<NodeAStar*>, decltype(nodePointerGreaterComparator)> minHeap(nodePointerGreaterComparator);

		NodeAStar* startNode = grid->NodeFromWorldPosition(startPos);
		NodeAStar* endNode = grid->NodeFromWorldPosition(endPos);

		/*std::wstringstream ss;
		ss << startNode->GetGridX() << ',' << startNode->GetGridY() << std::endl;
		OutputDebugString(ss.str().c_str());*/

		// add start node to the open set
		//openSet.push_back(startNode);
		startNode->SetInOpen(true);
		minHeap.push(startNode);
		//uopenSet.insert(startNode);

		// loop
		while (minHeap.size() > 0) {
			// .get() storeed pointer to object pointed to by the unique pointer
			//NodeAStar* currentNode = openSet[0];
			NodeAStar* currentNode = minHeap.top();
			//get the node in openset with the lowest fcost
			//int indexHolder = 0;
			////auto begin = std::chrono::high_resolution_clock::now();
			//
			//for (int i = 1; i < openSet.size(); i++) {
			//	if (openSet[i]->GetfCost() < currentNode->GetfCost() || // if lower fcost
			//		(openSet[i]->GetfCost() == currentNode->GetfCost() && // if same fcost, take the one with lower hcost
			//			openSet[i]->hCost < currentNode->hCost)) {
			//		currentNode = openSet[i];
			//		indexHolder = i;
			//	}
			//}

			/*auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed_seconds = end - begin;
			std::wstringstream ss;
			ss << "Tradional time taken : " << elapsed_seconds.count() * 1000 << "ms" << std::endl;
			OutputDebugString(ss.str().c_str());*/
			
		    //erase currentNode from openSet
			//openSet.erase(openSet.begin() + indexHolder);
			minHeap.pop();
			//openSet.erase(currentNode);
			currentNode->SetInOpen(false);
			// add currentNode to closedSet
			//closeSet.insert(currentNode);
			currentNode->SetInClosed(true);

			// if currentNode is equal to endNode, we have reached our destination
			if (currentNode == endNode) {
				RetracePath(startNode, endNode);
				return;
			}

			// find/update fcost of neighbours and add them to the openset
			for (NodeAStar* n : grid->GetNeighbours(currentNode)) {
				// if not traversable or already in closeSet, skip
				if (!n->GetWalkable() || n->GetInClosed()) {
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
						//openSet.push_back(n);
						n->SetInOpen(true);
					}
				}
			}
		}
	}

	// realized too late i could have used algorithm .find() maybe next time
	//bool InCloseSet(NodeAStar*& node) {
	//	/*std::vector<NodeAStar*>::iterator end = closeSet.end();
	//	for (std::vector<NodeAStar*>::iterator i = closeSet.begin(); i != end; std::advance(i, 1)) {
	//		if (node == *i) {
	//			return true;
	//		}
	//	}
	//	return false;*/
	//	if (closeSet.count(node) != 0) {
	//		return true;
	//	}
	//	return false;
	//}

	 //bool InOpenSet(NodeAStar*& node) {
		// /*std::vector<NodeAStar*>::iterator end = openSet.end();
		// for (std::vector<NodeAStar*>::iterator i = openSet.begin(); i != end; std::advance(i, 1)) {
		//	 if (node == *i) {
		//		 return true;
		//	 }
		// }
		// return false;*/
		// if (openSet.count(node) != 0) {
		//	 return true;
		// }
		// return false;
	 //}

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
	void RetracePath(NodeAStar* startNode, NodeAStar* endNode) {
		std::vector<NodeAStar*> path;
		NodeAStar* currentNode = endNode;

		// traversing backwards through parent
		while (startNode != currentNode) {
			currentNode->SetVisualize(true);
			path.push_back(currentNode);
			currentNode = currentNode->GetParent();
		}

		std::reverse(path.begin(), path.end());
	}
private:
	const std::shared_ptr<GridAStar>& grid;
	//std::vector<NodeAStar*> openSet;
	//std::vector<NodeAStar*> closeSet;
	//std::unordered_set<NodeAStar*> openSet;
	//std::unordered_set<NodeAStar*> closeSet;
};