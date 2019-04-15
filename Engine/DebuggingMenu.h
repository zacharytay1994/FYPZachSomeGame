#pragma once

#include "MenuSceneParent.h"
#include "ConsoleScene.h"

class DebuggingMenu : public MenuSceneParent {
public:
	DebuggingMenu(Graphics& gfx, std::stack<std::unique_ptr<MenuSceneParent>>& stackIn)
		:
		MenuSceneParent("Debugging Menu", "debuggingMenu1.bmp", gfx, stackIn, Vecf3(255.0f, 255.0f, 255.0f))
	{
		clickableRects.emplace_back(Rect(Vecf2(12.5f * perX, 85.0f * perY), 18.5f * perX, 10.0f * perY));
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
					if (consoleOut) {
						consoleOut = false;
					}
					else {
						consoleOut = true;
					}
				}
			}
		}
	}
	virtual void AddDraw() override {
		if (consoleOut) {
			consoleScene.Draw();
		}
	}
private:
	const Surface supSurfaces[2] = { Surface("debuggingMenu1.bmp"), Surface("debuggingMenu2.bmp") };
	Vecf2 mousePosition;
	ConsoleScene consoleScene = ConsoleScene(gfx);
	bool consoleOut = false;
};