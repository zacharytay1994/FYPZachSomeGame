#pragma once

#include "Rect.h"
#include "Graphics.h"

#include <memory>
#include <vector>

#include <sstream>
#include <string>

template <typename T>
class Quadtree {
public:
	template <typename T>
	class QtNode {
	public:
		QtNode(const Rect& boundary)
			:
			boundary(boundary)
		{}
		// sets a node state divided to true, and initializes its inner nodes
		void Subdivide() {
			float halfHeight = boundary.height / 4.0f;
			float halfWidth = boundary.width / 4.0f;
			northWest = std::make_unique<QtNode<T>>(Rect(Vecf2( boundary.rectCenter.x - halfWidth, boundary.rectCenter.y + halfHeight ), halfWidth * 2.0f, halfHeight * 2.0f));
			northEast = std::make_unique<QtNode<T>>(Rect(Vecf2( boundary.rectCenter.x + halfWidth, boundary.rectCenter.y + halfHeight ), halfWidth * 2.0f, halfHeight * 2.0f));
			southWest = std::make_unique<QtNode<T>>(Rect(Vecf2( boundary.rectCenter.x - halfWidth, boundary.rectCenter.y - halfHeight ), halfWidth * 2.0f, halfHeight * 2.0f));
			southEast = std::make_unique<QtNode<T>>(Rect(Vecf2( boundary.rectCenter.x + halfWidth, boundary.rectCenter.y - halfHeight ), halfWidth * 2.0f, halfHeight * 2.0f));
			divided = true;
		}
		// draw node boundary and held elements mainly for debugging purposes
		void DrawNode(const float& displaySize, const float& rootSize, Graphics& gfx) {
			Vecf2 screenOffSet = { gfx.ScreenHeight / 2.0f, gfx.ScreenHeight / 2.0f };
			gfx.DrawLine((topLeft / rootSize) * displaySize + screenOffSet, (topRight / rootSize) * displaySize + screenOffSet, Colors::Cyan);
			gfx.DrawLine(topLeft / rootSize * displaySize + screenOffSet, (bottomLeft / rootSize) * displaySize + screenOffSet, Colors::Cyan);
			gfx.DrawLine((bottomRight / rootSize) * displaySize + screenOffSet, (topRight / rootSize) * displaySize + screenOffSet, Colors::Cyan);
			gfx.DrawLine((bottomRight / rootSize) * displaySize + screenOffSet, (bottomLeft / rootSize) * displaySize + screenOffSet, Colors::Cyan);
			for (T* e : nodeArray) {
				if (!e->isColliding) {
					gfx.PutLargePixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x), int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y), Colors::Green, 2);
				}
				else {
					gfx.PutLargePixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x), int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y), Colors::Red, 2);
				}
			}
			if (divided) {
				// recursively draw inner nodes
				northWest->DrawNode(displaySize, rootSize, gfx);
				northEast->DrawNode(displaySize, rootSize, gfx);
				southWest->DrawNode(displaySize, rootSize, gfx);
				southEast->DrawNode(displaySize, rootSize, gfx);
			}
			else {
				return;
			}
		}
		// inserts an elements into a node,
		// if node is not full and not divided, add it to its element container,
		// if node is full and not divided, subdivide the node, and recursively move all elements into inner nodes
		// if node is full and divided, recursively insert into inner nodes
		void InsertElement(T* element) {
			if (nodeArray.size() < capacity && !divided) {
				nodeArray.push_back(element);
			}
			else {
				if (!divided) {
					nodeArray.push_back(element);
					Subdivide();
					// move points from parent node to subdivided child nodes
					for (T* e : nodeArray) {
						// check to see which quadrant element lies in and insert element into quadrant
						switch (IsIn(e)) {
						case 0:
							northWest->InsertElement(std::move(e));
							break;
						case 1:
							northEast->InsertElement(std::move(e));
							break;
						case 2:
							southWest->InsertElement(std::move(e));
							break;
						case 3:
							southEast->InsertElement(std::move(e));
							break;
						}
					}
				}
				else {
					// check to see which quadrant element lies in and insert element into quadrant
					switch (IsIn(element)) {
					case 0:
						northWest->InsertElement(element);
						break;
					case 1:
						northEast->InsertElement(element);
						break;
					case 2:
						southWest->InsertElement(element);
						break;
					case 3:
						southEast->InsertElement(element);
						break;
					}
				}
				
			}
		}
		// check if the elements position in world space, fall within a node's boundary
		int IsIn(T* e) {
			float xPos = e->GetSpawnLocationOffset().x;
			float zPos = e->GetSpawnLocationOffset().z;
			// top left quadrant
			if (xPos < boundary.rectCenter.x && zPos > boundary.rectCenter.y) {
				return 0;
			}
			// top right quadrant
			else if (xPos > boundary.rectCenter.x && zPos > boundary.rectCenter.y) {
				return 1;
			}
			// bottom left quadrant											 
			else if (xPos < boundary.rectCenter.x && zPos < boundary.rectCenter.y) {
				return 2;
			}
			// bottom right quadrant										  
			else if (xPos > boundary.rectCenter.x && zPos < boundary.rectCenter.y) {
				return 3;
			}
			else {
				return -1;
			}
		}
		// query if range overlaps with node boundary,
		// if node does overlap and is divided, recursively query inner nodes,
		// if node does overlap and is not divided, check with all elements held by that node to see if elements fall within range
		void QueryQt(Rect& range) {
			if (range.IsOverlapRect(boundary)) {
				// if not divided return points within boundary
				if (!divided) {
					for (T* e : nodeArray) {
						if (range.IsOverlap(Vecf2(e->GetSpawnLocationOffset().x, e->GetSpawnLocationOffset().z))) {
							e->isColliding = true;
						}
					}
				}
				// if divided query inner nodes
				else {
					northWest->QueryQt(range);
					northEast->QueryQt(range);
					southWest->QueryQt(range);
					southEast->QueryQt(range);
				}
			}
		}
		// another draw query mainly for debugging purposes (too lazy to create a function in graphics so its here)
		void DrawQueryRange(Rect& range, const float& displaySize, const float& rootSize, Graphics& gfx) {
			Vecf2 topLeft = { range.rectCenter.x - range.width / 2.0f,-(range.rectCenter.y + range.height / 2.0f) };
			Vecf2 topRight = { range.rectCenter.x + range.width / 2.0f,-(range.rectCenter.y + range.height / 2.0f) };
			Vecf2 bottomLeft = { range.rectCenter.x - range.width / 2.0f,-(range.rectCenter.y - range.height / 2.0f) };
			Vecf2 bottomRight = { range.rectCenter.x + range.width / 2.0f,-(range.rectCenter.y - range.height / 2.0f) };
			Vecf2 screenOffSet = { gfx.ScreenHeight / 2.0f, gfx.ScreenHeight / 2.0f };
			gfx.DrawLine((topLeft / rootSize) * displaySize + screenOffSet, (topRight / rootSize) * displaySize + screenOffSet, Colors::Red);
			gfx.DrawLine(topLeft / rootSize * displaySize + screenOffSet, (bottomLeft / rootSize) * displaySize + screenOffSet, Colors::Red);
			gfx.DrawLine((bottomRight / rootSize) * displaySize + screenOffSet, (topRight / rootSize) * displaySize + screenOffSet, Colors::Red);
			gfx.DrawLine((bottomRight / rootSize) * displaySize + screenOffSet, (bottomLeft / rootSize) * displaySize + screenOffSet, Colors::Red);
		}
	public:
		// rectangle aabb of node
		Rect boundary;
	private:
		// rectangle corner positions of node, mainly for debugging drawing purposes 
		Vecf2 topLeft = { boundary.rectCenter.x - boundary.width / 2.0f,-(boundary.rectCenter.y + boundary.height / 2.0f) };
		Vecf2 topRight = { boundary.rectCenter.x + boundary.width / 2.0f,-(boundary.rectCenter.y + boundary.height / 2.0f) };
		Vecf2 bottomLeft = { boundary.rectCenter.x - boundary.width / 2.0f,-(boundary.rectCenter.y - boundary.height / 2.0f) };
		Vecf2 bottomRight = { boundary.rectCenter.x + boundary.width / 2.0f,-(boundary.rectCenter.y - boundary.height / 2.0f) };
		// maximum number of elements per node before having to divide
		float capacity = 4;
		// boolean state whether node has been divided or not
		bool divided = false;
		// array that contains elements belonging to the node, its not actually and array of nodes, more of an array of elements
		// poor naming convention
		std::vector<T*> nodeArray;
		// inner subdivided nodes held
		std::unique_ptr<QtNode<T>> northWest;
		std::unique_ptr<QtNode<T>> northEast;
		std::unique_ptr<QtNode<T>> southWest;
		std::unique_ptr<QtNode<T>> southEast;
		// string stream for debugging purposes
		std::wstringstream ss;
	};
public:
	Quadtree(const float& size) 
		:
		rootNode(std::make_unique<QtNode<T>>(Rect(Vecf2(0.0f, 0.0f), size, size))),
		size(size)
	{}
	// draws the debugging display of a quadtree on screen
	void DrawNodeDebuggingDisplay(const float& displaySize, Graphics& gfx) {
		rootNode->DrawNode(displaySize, size, gfx);
		rootNode->DrawQueryRange(debugQueryRange, displaySize, size, gfx);
	}
	// inserts element into root node, i.e. start the recursive ball rolling of inserting into inner nodes
	void InsertElement(T* element) {
		// ensure element lies within world boundary
		if (element->GetSpawnLocationOffset().x > (rootNode->boundary.rectCenter.x - rootNode->boundary.width / 2.0f) && element->GetSpawnLocationOffset().x < (rootNode->boundary.rectCenter.x + rootNode->boundary.width / 2.0f) &&
			element->GetSpawnLocationOffset().z >(rootNode->boundary.rectCenter.y - rootNode->boundary.height / 2.0f) && element->GetSpawnLocationOffset().z < (rootNode->boundary.rectCenter.y + rootNode->boundary.height / 2.0f)) {
			rootNode->InsertElement(element);
		}
	}
	// resets the node by discarding the object being pointed to and replacing it with a new one
	// this is done per frame to refresh the quadtree for updated referencing
	void Reset() {
		rootNode.reset(new QtNode<T>(Rect(Vecf2(0.0f, 0.0f), size, size)));
		//ss << "root reset" << std::endl;
	}
	// queries the quadtree recursively through the nodes, and does stuff to elements that falls within range
	void QueryQt(Rect& queryRange) {
		rootNode->QueryQt(queryRange);
	}
	// sets query range mainly for debugging purposes
	void SetQueryRange(Rect& queryRangeIn) {
		debugQueryRange = queryRangeIn;
	}
private:
	std::unique_ptr<QtNode<T>> rootNode;
	// size of root boundary, i.e. the span of the quadtree in world/model size, centered on world position (0,0,0)
	float size;
	// debugging purposes
	Rect debugQueryRange = Rect({ 0.0f, 0.0f }, 0.1f, 0.1f);
	std::wstringstream ss;
};