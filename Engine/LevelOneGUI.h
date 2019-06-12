#pragma once

#include "MenuSceneParent.h"
#include "FontList.h"
#include "SurfaceDirectionalLighting.h"
#include "Console.h"

class LevelOneGUI : public MenuSceneParent {
public:
	LevelOneGUI(Graphics& gfx, std::stack<std::unique_ptr<MenuSceneParent>>& stackIn, std::vector<std::unique_ptr<Scene>>& scenes,
		const std::shared_ptr<FontList>& fontList)
		:
		MenuSceneParent("Level One GUI", "levelguione.bmp", gfx, stackIn, scenes, Vecf3(255.0f, 255.0f, 255.0f), fontList),
		surface("gameloopbackground2.bmp"),
		console(fontList->LevelGUIGameLoop, surface)
	{
		console.isGUI = true;
		console.Write("turretone.attack(1,2,3)");
		console.Write("something very long that exceeds the boundaries");
		for (int i = 0; i < 19; i++) {
			console.Write("turretone.attack(1,2,3)");
		}
		console.Write("something very long that exceeds the boundaries");
		//clickableRects.emplace_back(Rect(Vecf2(12.5f * perX, 85.0f * perY), 18.5f * perX, 10.0f * perY));
	}
	virtual void Update(Keyboard& kbd, Mouse& mouse, float dt) override {
		while (!mouse.IsEmpty()) {
			const auto e = mouse.Read();
			mousePosition = { (float)e.GetPosX(), (float)e.GetPosY() };

			// set surface to be drawn
			/*if (clickableRects[0].IsOverlap(mousePosition)) {
				surface = supSurfaces[1];
				eventID = 1;
			}
			else {
				surface = supSurfaces[0];
				eventID = 0;
			}*/

			// on click event
			if (OnClick(e)) {
			}
		}
		consoleScene.Update(kbd, mouse, dt);
	}
	virtual void AddDraw() override {
		/*if (consoleOut) {
			consoleScene.Draw();
		}*/
		console.DrawSurfaceGUI(consoleOneStartX, consoleOneStartY, consoleOneWidth, consoleOneHeight, gfx);
	}
private:
	// supplementary surface container
	const Surface supSurfaces[1] = { Surface("levelguione.bmp") };
	Vecf2 mousePosition;
	ConsoleScene consoleScene = ConsoleScene(gfx, fontList);
	bool consoleOut = false;
	// font writing variables
	Surface surface; // surface to alter
	Console<SurfaceDirectionalLighting> console;
	int test = 0;
	// console stuff
	int consoleOneStartX = 0 + test;
	int consoleOneStartY = int((((float)gfx.ScreenHeightDisplay / 3.0f)*2.0f)/3.0f) + test;
	int consoleOneWidth = gfx.ScreenWidthDisplay / 3 - test*2;
	int consoleOneHeight = int(((((float)gfx.ScreenHeightDisplay / 3.0f)*2.0f) / 3.0f)*2.0f) - test*2;
};