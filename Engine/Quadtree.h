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
				return;
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
			// make sure elements are within the world boundary
			if (xPos > boundary.rectCenter.x - boundary.width / 2.0f && xPos < boundary.rectCenter.x + boundary.width / 2.0f &&
				zPos > boundary.rectCenter.y - boundary.height / 2.0f && zPos < boundary.rectCenter.y + boundary.height / 2.0f) {
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
				return -1;
			}
			return -1;
		}
	private:
		Rect boundary;
		Vecf2 topLeft = { boundary.rectCenter.x - boundary.width / 2.0f,-(boundary.rectCenter.y + boundary.height / 2.0f) };
		Vecf2 topRight = { boundary.rectCenter.x + boundary.width / 2.0f,-(boundary.rectCenter.y + boundary.height / 2.0f) };
		Vecf2 bottomLeft = { boundary.rectCenter.x - boundary.width / 2.0f,-(boundary.rectCenter.y - boundary.height / 2.0f) };
		Vecf2 bottomRight = { boundary.rectCenter.x + boundary.width / 2.0f,-(boundary.rectCenter.y - boundary.height / 2.0f) };;
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
	}
	void InsertElement(T* element) {
		rootNode->InsertElement(element);
		/*ss << "element inserted" << std::endl;
		OutputDebugString(ss.str().c_str());*/
	}
	void Reset() {
		rootNode.reset(new QtNode<T>(Rect(Vecf2(0.0f, 0.0f), size, size)));
		//ss << "root reset" << std::endl;
	}
private:
	std::unique_ptr<QtNode<T>> rootNode;
	float size;
	std::wstringstream ss;
};