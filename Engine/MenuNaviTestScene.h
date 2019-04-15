#pragma once

#include "MenuSceneParent.h"

class MenuNaviTestScene : public MenuSceneParent {
public:
	MenuNaviTestScene(Graphics& gfx, std::stack<std::unique_ptr<MenuSceneParent>>& stackIn)
		:
		MenuSceneParent("Navi Test", "menunavi1.bmp", gfx, stackIn, Vecf3(255.0f, 255.0f, 255.0f))
	{
		clickableRects.emplace_back(Rect(Vecf2(50.0f * perX, 50.0f * perY), 30.0f * perX, 20.0f * perY));
	}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
		while (!mouse.IsEmpty()) {
			const auto e = mouse.Read();
			mousePosition = { (float)e.GetPosX(), (float)e.GetPosY() };

			// set surface to be drawn
			if (clickableRects[0].IsOverlap(mousePosition)) {
				surface = supSurfaces[1];
				eventID = 1;
			}
			else {
				surface = supSurfaces[0];
				eventID = 0;
			}

			// on click event
			if (OnClick(e)) {
				if (eventID == 1) {
					menuScenes.pop();
				}
			}
		}
	}
	virtual void AddDraw() {

	}
private:
	const Surface supSurfaces[2] = {Surface("menunavi1.bmp"), Surface("menunavi2.bmp")};
	Vecf2 mousePosition;
};