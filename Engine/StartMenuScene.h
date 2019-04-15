#pragma once

#include "MenuSceneParent.h"
#include "MenuNaviTestScene.h"

class StartMenuScene : public MenuSceneParent {
public:
	StartMenuScene(Graphics& gfx, std::stack<std::unique_ptr<MenuSceneParent>>& stackIn) 
		:
		MenuSceneParent("Start Menu", "startmenutest1.bmp", gfx, stackIn, Vecf3(255.0f, 255.0f, 255.0f))
	{
		// buttons
		clickableRects.emplace_back(Rect(Vecf2(50.0f * perX,50.0f * perY), 30.0f * perX, 20.0f * perY));
		clickableRects.emplace_back(Rect(Vecf2(50.0f * perX, 75.0f * perY), 30.0f * perX, 20.0f * perY));
		clickableRects.emplace_back(Rect(Vecf2(87.0f * perX, 18.0f * perY), 14.0f * perX, 24.0f * perY));
		clickableRects.emplace_back(Rect(Vecf2(86.0f * perX, 65.7f * perY), 27.0f * perX, 3.6f * perY));
		clickableRects.emplace_back(Rect(Vecf2(17.7f * perX, 38.8f * perY), 4.1f * perX, 7.4f * perY));
		clickableRects.emplace_back(Rect(Vecf2(7.8f * perX, 77.7f * perY), 3.1f * perX, 29.6f * perY));
	}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
		while (!mouse.IsEmpty()) {
			const Mouse::Event e = mouse.Read();
			mousePosition = { (float)e.GetPosX(), (float)e.GetPosY() };

			// set surface to be drawn
			if (clickableRects[0].IsOverlap(mousePosition)) {
				surface = supSurfaces[1];
				eventID = 1;
			}
			else if (clickableRects[1].IsOverlap(mousePosition)) {
				surface = supSurfaces[2];
				eventID = 2;
			}
			else if (clickableRects[2].IsOverlap(mousePosition)) {
				surface = supSurfaces[3];
				eventID = 3;
			}
			else if (clickableRects[3].IsOverlap(mousePosition)) {
				surface = supSurfaces[4];
			}
			else if (clickableRects[4].IsOverlap(mousePosition)) {
				surface = supSurfaces[5];
			}
			else if (clickableRects[5].IsOverlap(mousePosition)) {
				surface = supSurfaces[6];
			}
			else {
				surface = supSurfaces[0];
				eventID = 0;
			}

			// on click event
			if (OnClick(e)) {
				if (eventID == 1) {
					menuScenes.push(std::make_unique<MenuNaviTestScene>(gfx, menuScenes));
				}
				else if (eventID == 3) {
					testState = true;
				}
				else {
					menuScenes.pop();
				}
			}
		}
	}

	virtual void AddDraw() {
		if (testState) {
			gfx.DrawSprite(20, 20, Surface("test.bmp"));
		}
	}
private:
	const Surface supSurfaces[7] = { Surface("startmenutest1.bmp"), Surface("startmenutest2.bmp"), Surface("startmenutest3.bmp"),
	Surface("startmenutest4.bmp"), Surface("startmenutest5.bmp"), Surface("startmenutest6.bmp"), Surface("startmenutest7.bmp") };
	Vecf2 mousePosition;
	bool testState = false;
};
