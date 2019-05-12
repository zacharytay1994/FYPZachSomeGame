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
		void Subdivide() {
			float halfHeight = boundary.height / 4.0f;
			float halfWidth = boundary.width / 4.0f;
			northWest = std::make_unique<QtNode<T>>(Rect(Vecf2( boundary.rectCenter.x - halfWidth, boundary.rectCenter.y + halfHeight ), halfWidth * 2.0f, halfHeight * 2.0f));
			northEast = std::make_unique<QtNode<T>>(Rect(Vecf2( boundary.rectCenter.x + halfWidth, boundary.rectCenter.y + halfHeight ), halfWidth * 2.0f, halfHeight * 2.0f));
			southWest = std::make_unique<QtNode<T>>(Rect(Vecf2( boundary.rectCenter.x - halfWidth, boundary.rectCenter.y - halfHeight ), halfWidth * 2.0f, halfHeight * 2.0f));
			southEast = std::make_unique<QtNode<T>>(Rect(Vecf2( boundary.rectCenter.x + halfWidth, boundary.rectCenter.y - halfHeight ), halfWidth * 2.0f, halfHeight * 2.0f));
			divided = true;
		}
		void DrawNode(const float& displaySize, const float& rootSize, Graphics& gfx) {
			Vecf2 screenOffSet = { gfx.ScreenHeight / 2.0f, gfx.ScreenHeight / 2.0f };
			gfx.DrawLine((topLeft / rootSize) * displaySize + screenOffSet, (topRight / rootSize) * displaySize + screenOffSet, Colors::Cyan);
			gfx.DrawLine(topLeft / rootSize * displaySize + screenOffSet, (bottomLeft / rootSize) * displaySize + screenOffSet, Colors::Cyan);
			gfx.DrawLine((bottomRight / rootSize) * displaySize + screenOffSet, (topRight / rootSize) * displaySize + screenOffSet, Colors::Cyan);
			gfx.DrawLine((bottomRight / rootSize) * displaySize + screenOffSet, (bottomLeft / rootSize) * displaySize + screenOffSet, Colors::Cyan);
			for (T* e : nodeArray) {
				if (!e->isColliding) {
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x), int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y), Colors::Green);

					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) + 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y), Colors::Green);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x), int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) + 1, Colors::Green);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) + 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) + 1, Colors::Green);

					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) - 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y), Colors::Green);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x), int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) - 1, Colors::Green);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) - 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) - 1, Colors::Green);

					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) + 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) - 1, Colors::Green);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) - 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) + 1, Colors::Green);
				}
				else {
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x), int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y), Colors::Red);

					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) + 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y), Colors::Red);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x), int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y), Colors::Red);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) + 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) + 1, Colors::Red);

					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) - 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y), Colors::Red);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x), int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) - 1, Colors::Red);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) - 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) - 1, Colors::Red);

					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) + 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) - 1, Colors::Red);
					gfx.PutPixel(int((e->GetSpawnLocationOffset().x / rootSize) * displaySize + screenOffSet.x) - 1, int((-e->GetSpawnLocationOffset().z / rootSize) * displaySize + screenOffSet.y) + 1, Colors::Red);
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
						// check to see which quadrant element lies in
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
					// check to see which quadrant element lies in
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
		Rect boundary;
	private:
		Vecf2 topLeft = { boundary.rectCenter.x - boundary.width / 2.0f,-(boundary.rectCenter.y + boundary.height / 2.0f) };
		Vecf2 topRight = { boundary.rectCenter.x + boundary.width / 2.0f,-(boundary.rectCenter.y + boundary.height / 2.0f) };
		Vecf2 bottomLeft = { boundary.rectCenter.x - boundary.width / 2.0f,-(boundary.rectCenter.y - boundary.height / 2.0f) };
		Vecf2 bottomRight = { boundary.rectCenter.x + boundary.width / 2.0f,-(boundary.rectCenter.y - boundary.height / 2.0f) };
		float capacity = 4;
		bool divided = false;
		std::vector<T*> nodeArray;
		std::unique_ptr<QtNode<T>> northWest;
		std::unique_ptr<QtNode<T>> northEast;
		std::unique_ptr<QtNode<T>> southWest;
		std::unique_ptr<QtNode<T>> southEast;
		std::wstringstream ss;
	};
public:
	Quadtree(const float& size) 
		:
		rootNode(std::make_unique<QtNode<T>>(Rect(Vecf2(0.0f, 0.0f), size, size))),
		size(size)
	{}
	void DrawNodeDebuggingDisplay(const float& displaySize, Graphics& gfx) {
		rootNode->DrawNode(displaySize, size, gfx);
		rootNode->DrawQueryRange(debugQueryRange, displaySize, size, gfx);
	}
	void DrawQueryDebuggingDisplay(Rect& range, const float& displaySize, Graphics& gfx) {
		
	}
	void InsertElement(T* element) {
		// ensure element lies within world boundary
		if (element->GetSpawnLocationOffset().x > (rootNode->boundary.rectCenter.x - rootNode->boundary.width / 2.0f) && element->GetSpawnLocationOffset().x < (rootNode->boundary.rectCenter.x + rootNode->boundary.width / 2.0f) &&
			element->GetSpawnLocationOffset().z >(rootNode->boundary.rectCenter.y - rootNode->boundary.height / 2.0f) && element->GetSpawnLocationOffset().z < (rootNode->boundary.rectCenter.y + rootNode->boundary.height / 2.0f)) {
			rootNode->InsertElement(element);
		}
		/*ss << "element inserted" << std::endl;
		OutputDebugString(ss.str().c_str());*/
	}
	void Reset() {
		rootNode.reset(new QtNode<T>(Rect(Vecf2(0.0f, 0.0f), size, size)));
		//ss << "root reset" << std::endl;
	}
	void QueryQt(Rect& queryRange) {
		rootNode->QueryQt(queryRange);
	}
	void SetQueryRange(Rect& queryRangeIn) {
		debugQueryRange = queryRangeIn;
	}
private:
	std::unique_ptr<QtNode<T>> rootNode;
	float size;
	Rect debugQueryRange = Rect({ 0.0f, 0.0f }, 0.1f, 0.1f);
	std::wstringstream ss;
};