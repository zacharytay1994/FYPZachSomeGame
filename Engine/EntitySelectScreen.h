#pragma once

#include "MenuSceneParent.h"
#include "DebugWorld.h"

class EntitySelectScreen : public MenuSceneParent {
public:
	enum class EntitySelected {
		none = 0,
		turret = 1,
		enemy = 2,
		building = 3
	};
public:
	EntitySelectScreen(Graphics& gfx,
		std::stack<std::unique_ptr<MenuSceneParent>>& stackIn, std::vector<std::unique_ptr<Scene>>& scenes,
		const std::shared_ptr<FontList>& fontList)
		:
		MenuSceneParent("Entity Select Menu", "entityselectscreen.bmp", gfx, stackIn, scenes, Vecf3(255.0f, 255.0f, 255.0f), fontList)
		/*debugWorld(scene)*/
	{
		// buttons
		clickableRects.emplace_back(Rect(Vecf2(38.1f * perX, 75.0f * perY), 16.2f * perX, 25.0f * perY));
		clickableRects.emplace_back(Rect(Vecf2(62.5f * perX, 75.0f * perY), 16.2f * perX, 25.0f * perY));
		clickableRects.emplace_back(Rect(Vecf2(86.2f * perX, 75.0f * perY), 16.2f * perX, 25.0f * perY));
		clickableRects.emplace_back(Rect(Vecf2(12.5f * perX, 85.0f * perY), 18.5f * perX, 10.0f * perY));
	}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
		while (!mouse.IsEmpty()) {
			const Mouse::Event e = mouse.Read();
			mousePosition = { (float)e.GetPosX(), (float)e.GetPosY() };

			// set surface to be drawn
			if (clickableRects[0].IsOverlap(mousePosition)) {
				eventID = 1;
			}
			else if (clickableRects[1].IsOverlap(mousePosition)) {
				eventID = 2;
			}
			else if (clickableRects[2].IsOverlap(mousePosition)) {
				eventID = 3;
			}
			else if (clickableRects[3].IsOverlap(mousePosition)) {
				eventID = 4;
			}
			else {
				eventID = 0;
			}

			// on click event
			if (OnClick(e)) {
				switch (eventID) {
				case 1:
					surface = supSurfaces[1];
					(*scenes.begin())->mouseInteract.SetEntitySelected(static_cast<int>(EntitySelected::turret));
					break;
				case 2:
					surface = supSurfaces[2];
					(*scenes.begin())->mouseInteract.SetEntitySelected(static_cast<int>(EntitySelected::enemy));
					break;
				case 3:
					surface = supSurfaces[3];
					(*scenes.begin())->mouseInteract.SetEntitySelected(static_cast<int>(EntitySelected::building));
					break;
				case 4:
					menuScenes.pop();
					(*scenes.begin())->mouseInteract.SetEntitySelected(static_cast<int>(EntitySelected::none));
					break;
				}
				/*if (eventID == 1) {
					surface = supSurfaces[1];
				}
				else if (eventID == 2) {
					surface = supSurfaces[2];
				}
				else if (eventID == 3) {
					surface = supSurfaces[3];
				}
				else if (eventID == 4) {
					menuScenes.pop();
				}*/
			}
		}
	}

	virtual void AddDraw() {
	}
private:
	const Surface supSurfaces[4] = { Surface("entityselectscreen.bmp"), Surface("entityselectscreen2.bmp"),
		Surface("entityselectscreen3.bmp"), Surface("entityselectscreen4.bmp") };
	Vecf2 mousePosition;
	// scene reference
	//DebugWorld* debugWorld;
	// mouse interact variables
	int entitySelected = 0;
};
